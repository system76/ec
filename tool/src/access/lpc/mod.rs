const SMFI_CMD_BASE: u16 = 0xE00;
const SMFI_CMD_SIZE: usize = 0x100;

const SMFI_DBG_BASE: u16 = 0xF00;
const SMFI_DBG_SIZE: usize = 0x100;

const SMFI_CMD_CMD: u8 = 0x00;
const SMFI_CMD_RES: u8 = 0x01;
const SMFI_CMD_DATA: u8 = 0x02;

#[cfg(feature = "redox_hwio")]
pub use self::direct::AccessLpcDirect;
#[cfg(feature = "redox_hwio")]
mod direct;

#[cfg(all(feature = "std", target_os = "linux"))]
pub use self::linux::AccessLpcLinux;
#[cfg(all(feature = "std", target_os = "linux"))]
mod linux;
