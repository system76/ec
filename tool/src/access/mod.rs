use crate::Error;

#[cfg(feature = "hidapi")]
pub use self::hid::AccessHid;
#[cfg(feature = "hidapi")]
mod hid;

#[cfg(any(
    feature = "redox_hwio",
    all(feature = "std", target_os = "linux")
))]
pub use self::lpc::*;
#[cfg(any(
    feature = "redox_hwio",
    all(feature = "std", target_os = "linux")
))]
mod lpc;

/// Access method for running an EC command
pub trait Access {
    /// Sends a command using the access method. Only internal use is recommended
    unsafe fn command(&mut self, cmd: u8, data: &mut [u8]) -> Result<u8, Error>;

    /// The maximum size that can be provided for the data argument
    fn data_size(&self) -> usize;
}
