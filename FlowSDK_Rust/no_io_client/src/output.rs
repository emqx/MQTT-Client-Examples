// SPDX-License-Identifier: Apache-2.0
use std::io::{self, Write};

/// The host owns bytes removed from the engine until the socket accepts them.
#[derive(Default)]
pub struct Output {
    bytes: Vec<u8>,
    written: usize,
}

impl Output {
    pub fn is_empty(&self) -> bool {
        self.written == self.bytes.len()
    }

    pub fn replace(&mut self, bytes: Vec<u8>) {
        assert!(self.is_empty(), "must preserve the unsent tail");
        self.bytes = bytes;
        self.written = 0;
    }

    pub fn flush(&mut self, socket: &mut impl Write) -> io::Result<()> {
        while !self.is_empty() {
            match socket.write(&self.bytes[self.written..]) {
                Ok(0) => {
                    return Err(io::Error::new(
                        io::ErrorKind::WriteZero,
                        "socket write returned zero",
                    ))
                }
                Ok(n) => self.written += n,
                Err(error) if error.kind() == io::ErrorKind::Interrupted => continue,
                Err(error) if error.kind() == io::ErrorKind::WouldBlock => return Ok(()),
                Err(error) => return Err(error),
            }
        }
        self.bytes.clear();
        self.written = 0;
        Ok(())
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::collections::VecDeque;

    struct ShortWriter {
        actions: VecDeque<io::Result<usize>>,
        sent: Vec<u8>,
    }

    impl Write for ShortWriter {
        fn write(&mut self, bytes: &[u8]) -> io::Result<usize> {
            let n = self
                .actions
                .pop_front()
                .unwrap_or(Ok(bytes.len()))?
                .min(bytes.len());
            self.sent.extend_from_slice(&bytes[..n]);
            Ok(n)
        }
        fn flush(&mut self) -> io::Result<()> {
            Ok(())
        }
    }

    #[test]
    fn partial_writes_interrupted_and_would_block_preserve_binary_payload() {
        let mut output = Output::default();
        output.replace(vec![0, 255, 3, 4, 5]);
        let mut socket = ShortWriter {
            actions: VecDeque::from([
                Ok(2),
                Err(io::ErrorKind::Interrupted.into()),
                Err(io::ErrorKind::WouldBlock.into()),
                Ok(1),
                Ok(2),
            ]),
            sent: vec![],
        };
        output.flush(&mut socket).unwrap();
        assert!(!output.is_empty());
        assert_eq!(socket.sent, [0, 255]);
        output.flush(&mut socket).unwrap();
        assert!(output.is_empty());
        assert_eq!(socket.sent, [0, 255, 3, 4, 5]);
        output.replace(vec![0xe0, 0]); // DISCONNECT follows the fully written payload.
        output.flush(&mut socket).unwrap();
        assert_eq!(socket.sent, [0, 255, 3, 4, 5, 0xe0, 0]);
    }

    #[test]
    fn zero_write_is_a_connection_failure() {
        let mut output = Output::default();
        output.replace(vec![1]);
        let mut socket = ShortWriter {
            actions: VecDeque::from([Ok(0)]),
            sent: vec![],
        };
        assert_eq!(
            output.flush(&mut socket).unwrap_err().kind(),
            io::ErrorKind::WriteZero
        );
        assert!(!output.is_empty());
    }
}
