#![no_std]

pub use self::ec::Ec;
mod ec;

pub use self::error::Error;
mod error;

pub use self::firmware::Firmware;
mod firmware;

pub use self::legacy::EcLegacy;
mod legacy;

pub use self::pmc::Pmc;
mod pmc;

pub use self::spi::{Spi, SpiRom, SpiTarget};
mod spi;

pub use self::super_io::SuperIo;
mod super_io;

pub use self::timeout::Timeout;
mod timeout;
