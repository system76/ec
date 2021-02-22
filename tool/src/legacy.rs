// SPDX-License-Identifier: MIT

use crate::{
    Error,
    Pmc,
    SuperIo,
    Timeout,
};

/// Run some EC commands on previous proprietary firmware
pub struct EcLegacy<T: Timeout> {
    pub pmc: Pmc<T>,
}

impl<T: Timeout> EcLegacy<T> {
    /// Probes for EC using direct hardware access. Unsafe due to no mutual exclusion
    pub unsafe fn new(primary: bool, timeout: T) -> Result<Self, Error> {
        let mut sio = SuperIo::new(if primary { 0x2E } else { 0x4E });

        let id =
            (sio.read(0x20) as u16) << 8 |
            (sio.read(0x21) as u16);

        match id {
            0x5570 | 0x8587 => (),
            _ => return Err(Error::SuperIoId(id)),
        }

        //TODO: is there a good way to probe?

        Ok(Self {
            pmc: Pmc::new(if primary { 0x62 } else { 0x68 }, timeout),
        })
    }

    /// Read the EC firmware project, which uniquely identifies the board
    pub unsafe fn project(&mut self, data: &mut [u8]) -> Result<usize, Error> {
        let mut i = 0;
        self.pmc.command(0x92)?;
        while i < data.len() {
            data[i] = self.pmc.read()?;
            if data[i] == b'$' {
                break;
            }
            i += 1;
        }
        Ok(i)
    }

    /// Read the EC firmware version
    pub unsafe fn version(&mut self, data: &mut [u8]) -> Result<usize, Error> {
        // Prepend `1.` to version string
        let mut i = 0;
        if i < data.len() {
            data[i] = b'1';
            i += 1;
        }
        if i < data.len() {
            data[i] = b'.';
            i += 1;
        }

        self.pmc.command(0x93)?;
        while i < data.len() {
            data[i] = self.pmc.read()?;
            if data[i] == b'$' {
                break;
            }
            i += 1;
        }
        Ok(i)
    }
}
