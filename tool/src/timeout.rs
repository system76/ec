// SPDX-License-Identifier: MIT

#[cfg(feature = "std")]
use std::time::{Duration, Instant};

#[macro_export]
macro_rules! timeout {
    ($t:expr, $f:expr) => {{
        let mut result = Err($crate::Error::Timeout);
        while $t.running() {
            match $f {
                Ok(ok) => {
                    result = Ok(ok);
                    break;
                },
                Err(err) => match err {
                    $crate::Error::WouldBlock => (),
                    _ => {
                        result = Err(err);
                        break;
                    }
                },
            }
        }
        result
    }};
}

/// Timeout for use in blocking operations
pub trait Timeout {
    /// Reset the timeout to its initial state
    fn reset(&mut self);

    /// Check if timeout is still running
    fn running(&self) -> bool;
}

/// Timeout implemented using std::time
#[cfg(feature = "std")]
pub struct StdTimeout {
    instant: Instant,
    duration: Duration,
}

#[cfg(feature = "std")]
impl StdTimeout {
    /// Create a timeout with the specified duration
    pub fn new(duration: Duration) -> Self {
        StdTimeout {
            instant: Instant::now(),
            duration
        }
    }
}

#[cfg(feature = "std")]
impl Timeout for StdTimeout {
    fn reset(&mut self) {
        self.instant = Instant::now();
    }

    fn running(&self) -> bool {
        self.instant.elapsed() < self.duration
    }
}
