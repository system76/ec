// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2020 System76, Inc.

//! Library for accessing System76 ECs
//! First, construct an access method, using an object implementing the `Access` trait. Next, an Ec
//! object can be contructed, which exposes the command interface.
//!
//! There are some differences between targets and features that are listed below:
//!  - `AccessHid` requires the `hidapi` feature. Only functional on USB ECs.
//!  - `AccessLpcDirect` requires the `redox_hwio` feature and a nightly compiler. This method is
//!    only recommended for use in firmware with LPC ECs, as mutual exclusion is not guaranteed.
//!  - `AccessLpcLinux` requires the `std` feature and `linux` target_os. Recommended for LPC ECs,
//!    as this method can utilize mutual exclusion.
//!  - `EcLegacy`, `Pmc`, and `SuperIo` all require the `redox_hwio` feature and a nightly
//!    compiler. It is only recommended to use these in firmware, as mutual exclusion is not
//!    guaranteed.

#![allow(clippy::missing_safety_doc)]
#![allow(clippy::needless_range_loop)]
#![cfg_attr(not(feature = "std"), no_std)]

#[cfg(not(feature = "std"))]
extern crate alloc;

pub use self::access::*;
mod access;

pub use self::ec::{Ec, FanMode, SecurityState};
mod ec;

pub use self::error::Error;
mod error;

pub use self::firmware::Firmware;
mod firmware;

#[cfg(feature = "redox_hwio")]
pub use self::legacy::EcLegacy;
#[cfg(feature = "redox_hwio")]
mod legacy;

#[cfg(feature = "redox_hwio")]
pub use self::pmc::Pmc;
#[cfg(feature = "redox_hwio")]
mod pmc;

pub use self::spi::{Spi, SpiRom, SpiTarget};
mod spi;

#[cfg(feature = "redox_hwio")]
pub use self::super_io::SuperIo;
#[cfg(feature = "redox_hwio")]
mod super_io;

#[cfg(feature = "std")]
pub use self::timeout::StdTimeout;
pub use self::timeout::Timeout;
mod timeout;
