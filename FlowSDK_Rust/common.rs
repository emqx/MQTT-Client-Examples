// SPDX-License-Identifier: Apache-2.0
// Shared configuration only. The two clients own their separate lifecycle code.
use clap::Parser;
use flowsdk::mqtt_client::{MqttClientOptions, MqttMessage, OperationTimeouts};
use std::{env, error::Error, io, time::Duration, time::SystemTime};

pub type Result<T> = std::result::Result<T, Box<dyn Error + Send + Sync>>;
pub const QOS: u8 = 1;
pub const MAX_BUFFER: usize = 128 * 1024;

#[derive(Parser)]
#[command(about = "FlowSDK MQTT 5 TCP publish/subscribe example")]
struct Args {
    #[arg(long, default_value = "localhost")]
    host: String,
    #[arg(long, default_value_t = 1883, value_parser = clap::value_parser!(u16).range(1..))]
    port: u16,
    #[arg(long)]
    topic: Option<String>,
    #[arg(long)]
    client_id: Option<String>,
    #[arg(long, default_value = "Hello from FlowSDK (MQTT 5, QoS 1)")]
    payload: String,
    /// Whole-exchange deadline in seconds; shutdown has a separate deadline.
    #[arg(long, default_value_t = 10, value_parser = clap::value_parser!(u64).range(1..=300))]
    timeout: u64,
    #[arg(long, default_value_t = 15, value_parser = clap::value_parser!(u16).range(1..=3600))]
    keep_alive: u16,
}

pub struct Config {
    pub host: String,
    pub port: u16,
    pub topic: String,
    pub payload: Vec<u8>,
    pub options: MqttClientOptions,
    pub timeout: Duration,
}

impl Config {
    pub fn parse(mode: &str) -> Result<Self> {
        let args = Args::parse();
        let nonce = SystemTime::now()
            .duration_since(SystemTime::UNIX_EPOCH)?
            .as_nanos();
        let client_id = args
            .client_id
            .unwrap_or_else(|| format!("flowsdk-{mode}-{}-{nonce:x}", std::process::id()));
        let topic = args
            .topic
            .unwrap_or_else(|| format!("flowsdk/examples/{client_id}"));
        if topic.is_empty() || topic.contains(['#', '+', '\0']) || topic.len() > 1024 {
            return Err("topic must be a nonempty publish topic of at most 1024 bytes".into());
        }
        if client_id.is_empty() || client_id.contains('\0') || client_id.len() > 1024 {
            return Err("client ID must be nonempty and at most 1024 bytes".into());
        }
        if args.payload.len() > 16 * 1024 {
            return Err("example payload limit is 16 KiB".into());
        }
        if args.host.is_empty() || args.host.contains("://") {
            return Err("--host must be a hostname or IP address, without a URL scheme".into());
        }
        let host = args
            .host
            .trim_start_matches('[')
            .trim_end_matches(']')
            .to_string();
        let peer = if host.contains(':') {
            format!("[{host}]:{}", args.port)
        } else {
            format!("{host}:{}", args.port)
        };
        let timeout = Duration::from_secs(args.timeout);
        let mut options = MqttClientOptions::builder()
            .peer(peer)
            .client_id(client_id)
            .mqtt_version(5)
            .clean_start(true)
            .reconnect(false)
            .auto_ack(true)
            .keep_alive(args.keep_alive)
            .incoming_receive_maximum(16)
            .max_incoming_packet_size(64 * 1024)
            .max_incoming_buffer_bytes(MAX_BUFFER)
            .max_outgoing_buffer_bytes(MAX_BUFFER)
            .max_event_count(64)
            .max_outgoing_packet_count(64)
            .operation_timeouts(OperationTimeouts {
                connect: Some(timeout),
                publish: Some(timeout),
                subscribe: Some(timeout),
                unsubscribe: Some(timeout),
            })
            .build();
        // Public example credentials; set both variables to empty for anonymous use.
        let username = credential("MQTT_USERNAME", "emqx")?;
        let password = credential("MQTT_PASSWORD", "public")?;
        if let Some(username) = username {
            options = options.username(username);
        }
        if let Some(password) = password {
            options = options.password(password.into_bytes());
        }
        Ok(Self {
            host,
            port: args.port,
            topic,
            payload: args.payload.into_bytes(),
            options,
            timeout,
        })
    }

    pub fn check_message(&self, message: &MqttMessage) -> Result<bool> {
        if message.topic_name != self.topic {
            return Ok(false);
        }
        if message.payload != self.payload || message.qos != QOS {
            return Err("received payload or QoS does not match the published message".into());
        }
        println!(
            "Received matching payload ({} bytes)",
            message.payload.len()
        );
        Ok(true)
    }

    pub fn announce(&self, mode: &str) {
        println!("Mode: {mode}; MQTT 5 / TCP / QoS 1");
        println!("Broker: {}:{}; topic: {}", self.host, self.port, self.topic);
    }
}

fn credential(name: &str, default: &str) -> Result<Option<String>> {
    let value = match env::var(name) {
        Ok(value) => value,
        Err(env::VarError::NotPresent) => default.to_string(),
        Err(_) => return Err(format!("{name} must contain valid Unicode").into()),
    };
    Ok((!value.is_empty()).then_some(value))
}

pub fn timed_out() -> io::Error {
    io::Error::new(io::ErrorKind::TimedOut, "MQTT exchange deadline exceeded")
}
