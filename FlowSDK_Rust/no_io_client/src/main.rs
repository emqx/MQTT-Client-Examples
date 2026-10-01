// SPDX-License-Identifier: Apache-2.0
// Adapts the lifecycle shown in emqx/flowsdk v0.6.2 examples/no_io_pubsub.rs.
#[path = "../../common.rs"]
mod common;
mod output;

use common::{Config, Result, QOS};
use flowsdk::mqtt_client::{
    MqttEvent, NoIoMqttClient, PublishCommand, SubscribeCommand, UnsubscribeCommand,
};
use mio::{net::TcpStream, Events, Interest, Poll, Registry, Token};
use output::Output;
use std::{
    collections::VecDeque,
    io::{self, Read},
    net::{Shutdown, SocketAddr, ToSocketAddrs},
    time::Instant,
};

const SOCKET: Token = Token(0);

fn main() -> std::process::ExitCode {
    match run() {
        Ok(()) => std::process::ExitCode::SUCCESS,
        Err(error) => {
            eprintln!("Error: {error}");
            std::process::ExitCode::FAILURE
        }
    }
}

fn run() -> Result<()> {
    let mut config = Config::parse("no-io")?;
    config.announce("no-I/O engine (application owns sockets and timers)");
    // Resolve synchronously before starting the network deadline.
    let mut addresses = (config.host.as_str(), config.port)
        .to_socket_addrs()?
        .collect();
    let deadline = Instant::now() + config.timeout;
    let mut poll = Poll::new()?;
    let mut socket = connect_next(poll.registry(), &mut addresses)?;
    let mut engine = NoIoMqttClient::new(std::mem::take(&mut config.options));
    let result = event_loop(
        &mut poll,
        &mut socket,
        &mut addresses,
        &mut engine,
        &config,
        deadline,
    );
    if result.is_err() {
        // Inform the engine before dropping the failed transport. No automatic retry.
        engine.handle_connection_lost();
    }
    let _ = poll.registry().deregister(&mut socket);
    let _ = socket.shutdown(Shutdown::Both);
    result?;
    println!("SUCCESS: published, received, unsubscribed, and disconnected");
    Ok(())
}

fn connect_next(
    registry: &Registry,
    addresses: &mut VecDeque<SocketAddr>,
) -> io::Result<TcpStream> {
    let mut last_error = io::Error::new(io::ErrorKind::AddrNotAvailable, "no broker addresses");
    while let Some(address) = addresses.pop_front() {
        match TcpStream::connect(address) {
            Ok(mut socket) => {
                registry.register(&mut socket, SOCKET, Interest::READABLE | Interest::WRITABLE)?;
                return Ok(socket);
            }
            Err(error) => last_error = error,
        }
    }
    Err(last_error)
}

#[derive(Default)]
struct Exchange {
    subscribed: Option<u16>,
    published: Option<u16>,
    unsubscribed: Option<u16>,
    acknowledged: bool,
    received: bool,
    disconnecting: bool,
}

impl Exchange {
    fn handle(
        &mut self,
        events: Vec<MqttEvent>,
        engine: &mut NoIoMqttClient,
        config: &Config,
    ) -> Result<()> {
        for event in events {
            match event {
                MqttEvent::Connected(result) => {
                    if !result.is_success() {
                        return Err(format!("CONNECT rejected: {result:?}").into());
                    }
                    println!("Connected");
                    self.subscribed =
                        Some(engine.subscribe(SubscribeCommand::single(&config.topic, QOS))?);
                }
                MqttEvent::Subscribed(result) if Some(result.packet_id) == self.subscribed => {
                    if result.reason_codes != [QOS] {
                        return Err(format!("SUBACK did not grant QoS 1: {result:?}").into());
                    }
                    println!("Subscribed (QoS 1)");
                    self.published = engine.publish(PublishCommand::simple(
                        &config.topic,
                        config.payload.clone(),
                        QOS,
                        false,
                    ))?;
                }
                MqttEvent::Published(result) if result.packet_id == self.published => {
                    if !result.is_success() {
                        return Err(format!("PUBLISH rejected: {result:?}").into());
                    }
                    self.acknowledged = true;
                    println!("Publish acknowledged (PUBACK)");
                }
                MqttEvent::MessageReceived(message) => {
                    self.received |= config.check_message(&message)?;
                }
                MqttEvent::Unsubscribed(result) if Some(result.packet_id) == self.unsubscribed => {
                    if result.reason_codes.len() != 1 || !result.is_success() {
                        return Err(format!("UNSUBACK rejected: {result:?}").into());
                    }
                    println!("Unsubscribed");
                    self.disconnecting = true;
                    engine.disconnect()?;
                }
                MqttEvent::Error(error) | MqttEvent::OperationFailed { error, .. } => {
                    return Err(error.into())
                }
                MqttEvent::Disconnected(_) if self.disconnecting => {}
                MqttEvent::Disconnected(_) | MqttEvent::DisconnectReceived { .. } => {
                    return Err("broker disconnected before exchange completed".into());
                }
                _ => {}
            }
        }
        if self.acknowledged && self.received && self.unsubscribed.is_none() {
            self.unsubscribed = Some(
                engine.unsubscribe(UnsubscribeCommand::from_topics(vec![config.topic.clone()]))?,
            );
        }
        Ok(())
    }
}

// The application drives socket I/O and time; the engine only processes MQTT state.
// Exchange turns protocol events into the next subscribe/publish/disconnect command.
fn event_loop(
    poll: &mut Poll,
    socket: &mut TcpStream,
    addresses: &mut VecDeque<SocketAddr>,
    engine: &mut NoIoMqttClient,
    config: &Config,
    deadline: Instant,
) -> Result<()> {
    // This tracks TCP completion. MQTT becomes connected only after CONNACK.
    let mut connected = false;
    // connect_next() initially watches writable readiness for TCP completion.
    let mut watching_writes = true;
    let mut exchange = Exchange::default();
    let mut output = Output::default();
    let mut ready = Events::with_capacity(8);
    loop {
        let now = Instant::now();
        if now >= deadline {
            return Err(common::timed_out().into());
        }
        if connected {
            advance_engine(engine, &mut exchange, config, now)?;
            flush_output(engine, &mut output, socket)?;
            // Draining output can make more packets available; send them before waiting.
            if output.is_empty() && engine.has_pending_output() {
                continue;
            }
            // Finish only after DISCONNECT leaves both the engine and our socket buffer.
            if exchange.disconnecting && output.is_empty() {
                return Ok(());
            }
            update_write_interest(
                poll.registry(),
                socket,
                &mut watching_writes,
                !output.is_empty(),
            )?;
        }
        // Wait for readiness or the earlier of the engine and application deadlines.
        let wake_at = if connected {
            engine
                .next_tick_at()
                .map_or(deadline, |tick| tick.min(deadline))
        } else {
            deadline
        };
        match poll.poll(
            &mut ready,
            Some(wake_at.saturating_duration_since(Instant::now())),
        ) {
            Ok(()) => {}
            // Retry after a signal, recomputing timers rather than using stale events.
            Err(error) if error.kind() == io::ErrorKind::Interrupted => continue,
            Err(error) => return Err(error.into()),
        }
        // Timer wakeups may have no events. Writable wakeups lead back to flush_output().
        for event in &ready {
            if event.token() != SOCKET {
                continue;
            }
            if !connected {
                match finish_connect(poll.registry(), socket, addresses)? {
                    ConnectState::Pending => continue,
                    ConnectState::Replaced => {
                        watching_writes = true;
                        // Remaining events belong to the old socket; wait for the new one.
                        break;
                    }
                    ConnectState::Connected => {
                        connected = true;
                        // Queue MQTT CONNECT; the next iteration sends it over TCP.
                        engine.connect()?;
                    }
                }
            }
            if event.is_readable() || event.is_read_closed() || event.is_error() {
                read_packets(socket, engine, &mut exchange, config, deadline)?;
            }
            if event.is_write_closed() && !exchange.disconnecting {
                return Err("broker closed the socket's write side".into());
            }
        }
    }
}

fn advance_engine(
    engine: &mut NoIoMqttClient,
    exchange: &mut Exchange,
    config: &Config,
    now: Instant,
) -> Result<()> {
    // Keepalive and operation deadlines must advance even when no socket data arrives.
    let events = engine.handle_tick(now);
    exchange.handle(events, engine, config)?;
    // Commands may create more events; input/tick calls already return their own events.
    let events = engine.take_events();
    exchange.handle(events, engine, config)
}

fn flush_output(
    engine: &mut NoIoMqttClient,
    output: &mut Output,
    socket: &mut TcpStream,
) -> io::Result<()> {
    // Never overwrite the unsent tail with the next batch of MQTT packets.
    if output.is_empty() {
        output.replace(engine.take_outgoing());
    }
    // Try writing now; flush() preserves remaining bytes if the socket would block.
    output.flush(socket)
}

fn update_write_interest(
    registry: &Registry,
    socket: &mut TcpStream,
    watching_writes: &mut bool,
    want_writes: bool,
) -> io::Result<()> {
    // Watching an idle writable socket can make poll spin. Always keep read interest.
    if want_writes != *watching_writes {
        let interest = if want_writes {
            Interest::READABLE | Interest::WRITABLE
        } else {
            Interest::READABLE
        };
        registry.reregister(socket, SOCKET, interest)?;
        *watching_writes = want_writes;
    }
    Ok(())
}

enum ConnectState {
    Pending,
    Connected,
    Replaced,
}

fn finish_connect(
    registry: &Registry,
    socket: &mut TcpStream,
    addresses: &mut VecDeque<SocketAddr>,
) -> io::Result<ConnectState> {
    // Nonblocking connect readiness may indicate success or failure. Check errors first.
    if let Some(error) = socket.take_error()? {
        if addresses.is_empty() {
            return Err(error);
        }
        registry.deregister(socket)?;
        *socket = connect_next(registry, addresses)?;
        return Ok(ConnectState::Replaced);
    }
    match socket.peer_addr() {
        Ok(_) => {
            socket.set_nodelay(true)?;
            Ok(ConnectState::Connected)
        }
        Err(error)
            if matches!(
                error.kind(),
                io::ErrorKind::NotConnected | io::ErrorKind::WouldBlock
            ) =>
        {
            Ok(ConnectState::Pending)
        }
        Err(error) => Err(error),
    }
}

fn read_packets(
    socket: &mut TcpStream,
    engine: &mut NoIoMqttClient,
    exchange: &mut Exchange,
    config: &Config,
    deadline: Instant,
) -> Result<()> {
    let mut input = [0_u8; 8192];
    // Drain reads until WouldBlock; one readiness event may cover multiple packets.
    loop {
        // A busy peer must not prevent the application deadline from expiring.
        if Instant::now() >= deadline {
            return Err(common::timed_out().into());
        }
        match socket.read(&mut input) {
            Ok(0) => return Err("broker closed the TCP connection".into()),
            Ok(n) => {
                // TCP chunks may split MQTT packets. The engine buffers fragments,
                // parses complete packets, and queues automatic acknowledgements.
                let events = engine.handle_incoming(&input[..n]);
                exchange.handle(events, engine, config)?;
                if exchange.disconnecting {
                    // Return to the write phase to flush the final packets.
                    return Ok(());
                }
            }
            Err(error) if error.kind() == io::ErrorKind::Interrupted => continue,
            Err(error) if error.kind() == io::ErrorKind::WouldBlock => return Ok(()),
            Err(error) => return Err(error.into()),
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use flowsdk::mqtt_client::MqttClientOptions;
    use std::time::Duration;

    #[test]
    fn engine_accepts_fragmented_input_and_ticks_without_a_runtime_or_socket() {
        let options = MqttClientOptions::builder()
            .client_id("no-io-test")
            .mqtt_version(5)
            .keep_alive(1)
            .auto_ack(true)
            .reconnect(false)
            .build();
        let mut engine = NoIoMqttClient::new(options);
        engine.connect().unwrap();
        assert_eq!(engine.take_outgoing()[0], 0x10);
        for byte in [0x20, 3, 0, 0] {
            assert!(engine.handle_incoming(&[byte]).is_empty());
        }
        assert!(matches!(
            engine.handle_incoming(&[0]).as_slice(),
            [MqttEvent::Connected(_)]
        ));
        // Incoming QoS 1 binary message: topic "t", packet ID 7, empty properties.
        let packet = [0x32, 8, 0, 1, b't', 0, 7, 0, 0, 255];
        let mut messages = vec![];
        for byte in packet {
            messages.extend(engine.handle_incoming(&[byte]));
        }
        assert!(matches!(messages.as_slice(),
            [MqttEvent::PublishReceived { packet_id: Some(7), .. }, MqttEvent::MessageReceived(message)]
            if message.payload == [0, 255]
        ));
        assert_eq!(engine.take_outgoing()[0], 0x40); // Auto PUBACK.
        assert!(engine.take_events().is_empty()); // Events already consumed once.
        engine.handle_tick(Instant::now() + Duration::from_secs(2));
        assert_eq!(engine.take_outgoing(), [0xc0, 0]); // PINGREQ driven by caller time.
        engine.handle_incoming(&[0xd0, 0]);
        engine.disconnect().unwrap();
        assert_eq!(engine.take_outgoing()[0], 0xe0);
    }
}
