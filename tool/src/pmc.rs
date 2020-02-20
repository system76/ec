use hwio::{Io, Pio};

pub struct Pmc {
    data: Pio<u8>,
    cmd: Pio<u8>,
}

impl Pmc {
    /// Create a new PMC instance. `base` identifies the data port. The command
    /// port will be `base + 4`
    pub fn new(base: u16) -> Self {
        Self {
            data: Pio::new(base),
            cmd: Pio::new(base + 4),
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
    pub unsafe fn command(&mut self, data: u8) -> Option<()> {
        if self.can_write() {
            self.cmd.write(data);
            Some(())
        } else {
            None
        }
    }

    /// Read data from the PMC. Returns None if unable to read
    pub unsafe fn read(&mut self) -> Option<u8> {
        if self.can_read() {
            Some(self.data.read())
        } else {
            None
        }
    }

    /// Write data to the PMC. Returns false if unable to write
    pub unsafe fn write(&mut self, data: u8) -> Option<()> {
        if self.can_write() {
            self.data.write(data);
            Some(())
        } else {
            None
        }
    }
}
