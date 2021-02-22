// SPDX-License-Identifier: MIT

use hwio::{Io, Pio};

use crate::{
    Error,
    Timeout,
    timeout
};

/// Standard ACPI EC interface
pub struct Pmc<T: Timeout> {
    data: Pio<u8>,
    cmd: Pio<u8>,
    timeout: T,
}

impl<T: Timeout> Pmc<T> {
    /// Create a new ACPI EC instance using direct hardware access. `base` identifies the data
    /// port. The command port will be `base + 4`. Unsafe due to no mutual exclusion
    pub unsafe fn new(base: u16, timeout: T) -> Self {
        Self {
            data: Pio::new(base),
            cmd: Pio::new(base + 4),
            timeout,
        }
    }

    /// Returns true if PMC can be read from
    pub unsafe fn can_read(&mut self) -> bool {
        self.cmd.read() & 1 == 1
    }

    /// Returns true if PMC can be written to
    pub unsafe fn can_write(&mut self) -> bool {
        self.cmd.read() & 2 == 0
    }

    /// Write a command to the PMC. Returns None if unable to write
    pub unsafe fn command(&mut self, data: u8) -> Result<(), Error> {
        if self.can_write() {
            self.cmd.write(data);
            Ok(())
        } else {
            Err(Error::WouldBlock)
        }
    }

    /// Read data from the PMC. Returns None if unable to read
    pub unsafe fn read(&mut self) -> Result<u8, Error> {
        if self.can_read() {
            Ok(self.data.read())
        } else {
            Err(Error::WouldBlock)
        }
    }

    /// Write data to the PMC. Returns false if unable to write
    pub unsafe fn write(&mut self, data: u8) -> Result<(), Error> {
        if self.can_write() {
            self.data.write(data);
            Ok(())
        } else {
            Err(Error::WouldBlock)
        }
    }

    /// Read from the ACPI region, at a specific address
    pub unsafe fn acpi_read(&mut self, addr: u8) -> Result<u8, Error> {
        self.timeout.reset();

        timeout!(self.timeout, self.command(0x80))?;
        timeout!(self.timeout, self.write(addr))?;
        timeout!(self.timeout, self.read())
    }
}
