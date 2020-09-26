pub(crate) const SMFI_CMD_BASE: u16 = 0xE00;
pub(crate) const SMFI_CMD_SIZE: usize = 0x100;

pub(crate) const SMFI_DBG_BASE: u16 = 0xF00;
pub(crate) const SMFI_DBG_SIZE: usize = 0x100;

pub(crate) const SMFI_CMD_CMD: u8 = 0x00;
pub(crate) const SMFI_CMD_RES: u8 = 0x01;
pub(crate) const SMFI_CMD_DATA: u8 = 0x02;

pub use self::direct::AccessLpcDirect;
mod direct;

#[cfg(all(feature = "std", target_os = "linux"))]
pub use self::linux::AccessLpcLinux;
#[cfg(all(feature = "std", target_os = "linux"))]
mod linux;
