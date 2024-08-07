// SPDX-License-Identifier: MIT

/// Errors returned by operations
#[derive(Debug)]
pub enum Error {
    /// Data length is too large
    DataLength(usize),
    /// Operation not supported
    NotSupported,
    /// A parameter was invalid
    Parameter,
    /// EC protocol returned an error result
    Protocol(u8),
    /// EC protocol signature did not match
    Signature((u8, u8)),
    /// Super I/O ID did not match
    SuperIoId(u16),
    /// Blocking operation timed out
    Timeout,
    /// Unexpected data from EC
    Verify,
    /// EC protocol version is unsupported
    Version(u8),
    /// Indicates that a blocking operation would block
    WouldBlock,
    /// Encountered a std::io::Error
    #[cfg(feature = "std")]
    Io(std::io::Error),
    /// Encountered a hidapi::Error
    #[cfg(feature = "hidapi")]
    Hid(hidapi::HidError),
    /// Writing to flash is disabled
    WriteLocked,
}

#[cfg(feature = "std")]
impl From<std::io::Error> for Error {
    fn from(error: std::io::Error) -> Self {
        Self::Io(error)
    }
}

#[cfg(feature = "hidapi")]
impl From<hidapi::HidError> for Error {
    fn from(error: hidapi::HidError) -> Self {
        Self::Hid(error)
    }
}
