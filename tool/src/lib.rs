// SPDX-License-Identifier: MIT

//! Library for accessing System76 ECs
//! First, construct an access method, using an object implementing the `Access` trait. Next, an Ec
//! object can be contructed, which exposes the command interface.
//!
//! There are some differences between targets and features that are listed below:
//!  - `AccessHid` requires the `hidapi` feature. Only functional on USB ECs.
//!  - `AccessLpcDirect` requires the `redox_hwio` feature and an x86 target. This method is
//!    only recommended for use in firmware with LPC ECs, as mutual exclusion is not guaranteed.
//!  - `AccessLpcLinux` requires the `std` feature, `linux` target_os, and x86 target. Recommended
//!    for LPC ECs, as this method can utilize mutual exclusion.
//!  - `EcLegacy`, `Pmc`, and `SuperIo` all require the `redox_hwio` feature and an x86 target.
//!    It is only recommended to use these in firmware, as mutual exclusion is not guaranteed.

#![allow(clippy::missing_safety_doc)]
#![allow(clippy::needless_range_loop)]
#![cfg_attr(not(feature = "std"), no_std)]

#[cfg(not(feature = "std"))]
extern crate alloc;

pub use self::access::*;
mod access;

pub use self::ec::{Ec, SecurityState};
mod ec;

pub use self::error::Error;
mod error;

pub use self::firmware::Firmware;
mod firmware;

#[cfg(all(
    feature = "redox_hwio",
    any(target_arch = "x86", target_arch = "x86_64")
))]
pub use self::legacy::EcLegacy;
#[cfg(all(
    feature = "redox_hwio",
    any(target_arch = "x86", target_arch = "x86_64")
))]
mod legacy;

#[cfg(all(
    feature = "redox_hwio",
    any(target_arch = "x86", target_arch = "x86_64")
))]
pub use self::pmc::Pmc;
#[cfg(all(
    feature = "redox_hwio",
    any(target_arch = "x86", target_arch = "x86_64")
))]
mod pmc;

pub use self::spi::{Spi, SpiRom, SpiTarget};
mod spi;

#[cfg(all(
    feature = "redox_hwio",
    any(target_arch = "x86", target_arch = "x86_64")
))]
pub use self::super_io::SuperIo;
#[cfg(all(
    feature = "redox_hwio",
    any(target_arch = "x86", target_arch = "x86_64")
))]
mod super_io;

#[cfg(feature = "std")]
pub use self::timeout::StdTimeout;
pub use self::timeout::Timeout;
mod timeout;
