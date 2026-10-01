// SPDX-License-Identifier: Apache-2.0
// Adapts the lifecycle shown in emqx/flowsdk v0.6.2 examples/async_pubsub.rs.
#[path = "../../common.rs"]
mod common;

use common::{Config, Result, QOS};
use flowsdk::mqtt_client::{
    MqttClientError, MqttMessage, TokioAsyncClientConfig, TokioAsyncMqttClient,
    TokioMqttEventHandler,
};
use std::sync::{
    atomic::{AtomicBool, Ordering},
    Arc,
};
use tokio::{sync::mpsc, time::timeout};

enum Event {
    Message(MqttMessage),
    Error(MqttClientError),
    Disconnected,
}

struct Handler {
    events: mpsc::Sender<Event>,
    overflow: Arc<AtomicBool>,
}

impl Handler {
    fn send(&self, event: Event) {
        // These callbacks run on the networking worker: never wait for the consumer.
        if let Err(mpsc::error::TrySendError::Full(_)) = self.events.try_send(event) {
            self.overflow.store(true, Ordering::Relaxed);
        }
    }
}

#[async_trait::async_trait]
impl TokioMqttEventHandler for Handler {
    async fn on_message_received(&mut self, message: &MqttMessage) {
        self.send(Event::Message(message.clone()));
    }
    async fn on_error(&mut self, error: &MqttClientError) {
        self.send(Event::Error(error.clone()));
    }
    async fn on_disconnected(&mut self, _: Option<u8>) {
        self.send(Event::Disconnected);
    }
}

#[tokio::main]
async fn main() -> std::process::ExitCode {
    match run().await {
        Ok(()) => std::process::ExitCode::SUCCESS,
        Err(error) => {
            eprintln!("Error: {error}");
            std::process::ExitCode::FAILURE
        }
    }
}

async fn run() -> Result<()> {
    let mut config = Config::parse("ready")?;
    config.announce("ready client (Tokio owns networking and timers)");
    let millis = config.timeout.as_millis() as u64;
    let worker = TokioAsyncClientConfig::builder()
        .auto_reconnect(false)
        .buffer_messages(false)
        .command_queue_size(16)
        .connect_timeout_ms(millis)
        .subscribe_timeout_ms(millis)
        .publish_ack_timeout_ms(millis)
        .unsubscribe_timeout_ms(millis)
        .default_operation_timeout_ms(millis)
        .build();
    let (tx, mut events) = mpsc::channel(32);
    let overflow = Arc::new(AtomicBool::new(false));
    let handler = Handler {
        events: tx,
        overflow: overflow.clone(),
    };
    let client = TokioAsyncMqttClient::new(
        std::mem::take(&mut config.options),
        Box::new(handler),
        worker,
    )
    .await?;

    // This application task awaits operations; the SDK worker continues doing I/O.
    let result = match timeout(config.timeout, exchange(&client, &config, &mut events)).await {
        Ok(result) => result,
        Err(_) => Err(common::timed_out().into()),
    };
    // Always join the worker, including when an operation failed or timed out.
    let shutdown = timeout(config.timeout, client.shutdown()).await;
    result?;
    shutdown.map_err(|_| "client shutdown deadline exceeded")??;
    if overflow.load(Ordering::Relaxed) {
        return Err("application event queue filled; refusing to report success".into());
    }
    println!("SUCCESS: published, received, unsubscribed, and disconnected");
    Ok(())
}

async fn exchange(
    client: &TokioAsyncMqttClient,
    config: &Config,
    events: &mut mpsc::Receiver<Event>,
) -> Result<()> {
    let connected = client.connect_sync().await?;
    if !connected.is_success() {
        return Err(format!("CONNECT rejected: {connected:?}").into());
    }
    println!("Connected");
    let subscribed = client.subscribe_sync(&config.topic, QOS).await?;
    if subscribed.reason_codes != [QOS] {
        return Err(format!("SUBACK did not grant QoS 1: {subscribed:?}").into());
    }
    println!("Subscribed (QoS 1)");
    let published = client
        .publish_sync(&config.topic, &config.payload, QOS, false)
        .await?;
    if !published.is_success() {
        return Err(format!("PUBLISH rejected: {published:?}").into());
    }
    println!("Publish acknowledged (PUBACK)");
    loop {
        match events
            .recv()
            .await
            .ok_or("application event channel closed")?
        {
            Event::Message(message) if config.check_message(&message)? => break,
            Event::Message(_) => {}
            Event::Error(error) => return Err(error.into()),
            Event::Disconnected => return Err("disconnected before receiving the message".into()),
        }
    }
    let unsubscribed = client.unsubscribe_sync(vec![&config.topic]).await?;
    if unsubscribed.reason_codes.len() != 1 || !unsubscribed.is_success() {
        return Err(format!("UNSUBACK rejected: {unsubscribed:?}").into());
    }
    println!("Unsubscribed");
    client.disconnect_sync().await?;
    Ok(())
}
