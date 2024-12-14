// SPDX-License-Identifier: MIT

use hwio::{Io, Pio};

use crate::{timeout, Access, Error, SuperIo, Timeout};

use super::*;

/// Use direct hardware access. Unsafe due to not having mutual exclusion
pub struct AccessLpcDirect<T: Timeout + Send + 'static> {
    cmd: u16,
    dbg: u16,
    timeout: T,
}

impl<T: Timeout + Send> AccessLpcDirect<T> {
    /// Checks that Super I/O ID matches and then returns access object
    pub unsafe fn new(timeout: T) -> Result<Self, Error> {
        // Make sure EC ID matches
        let mut sio = SuperIo::new(0x2E);
        let id = (sio.read(0x20) as u16) << 8 | (sio.read(0x21) as u16);
        match id {
            0x5570 | 0x8587 => (),
            _ => return Err(Error::SuperIoId(id)),
        }

        Ok(Self {
            cmd: SMFI_CMD_BASE,
            dbg: SMFI_DBG_BASE,
            timeout,
        })
    }

    /// Read from the command space
    unsafe fn read_cmd(&mut self, addr: u8) -> u8 {
        Pio::<u8>::new(self.cmd + (addr as u16)).read()
    }

    /// Write to the command space
    unsafe fn write_cmd(&mut self, addr: u8, data: u8) {
        Pio::<u8>::new(self.cmd + (addr as u16)).write(data)
    }

    /// Read from the debug space
    //TODO: better public interface
    pub unsafe fn read_debug(&mut self, addr: u8) -> u8 {
        Pio::<u8>::new(self.dbg + (addr as u16)).read()
    }

    /// Returns Ok if a command can be sent
    unsafe fn command_check(&mut self) -> Result<(), Error> {
        if self.read_cmd(SMFI_CMD_CMD) == 0 {
            Ok(())
        } else {
            Err(Error::WouldBlock)
        }
    }
}

impl<T: Timeout + Send> Access for AccessLpcDirect<T> {
    unsafe fn command(&mut self, cmd: u8, data: &mut [u8]) -> Result<u8, Error> {
        // Test data length
        if data.len() > self.data_size() {
            return Err(Error::DataLength(data.len()));
        }

        // All previous commands should be finished
        self.command_check()?;

        // Write data bytes, index should be valid due to length test above
        for i in 0..data.len() {
            self.write_cmd(i as u8 + SMFI_CMD_DATA, data[i]);
        }

        // Write command byte, which starts command
        self.write_cmd(SMFI_CMD_CMD, cmd as u8);

        // Wait for command to finish with timeout
        self.timeout.reset();
        timeout!(self.timeout, self.command_check())?;

        // Read data bytes, index should be valid due to length test above
        for i in 0..data.len() {
            data[i] = self.read_cmd(i as u8 + SMFI_CMD_DATA);
        }

        // Return response byte
        Ok(self.read_cmd(SMFI_CMD_RES))
    }

    fn data_size(&self) -> usize {
        SMFI_CMD_SIZE - SMFI_CMD_DATA as usize
    }
}
