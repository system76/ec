use hwio::{Io, Pio};

/// Super I/O interface provided by LPC ECs
pub struct SuperIo {
    addr: Pio<u8>,
    data: Pio<u8>,
}

impl SuperIo {
    /// Create a new SuperIo using direct hardware access. `base` identifies the address port. The
    /// data port will be `base + 1`. Unsafe due to no mutual exclusion
    pub unsafe fn new(base: u16) -> Self {
        Self {
            addr: Pio::new(base),
            data: Pio::new(base + 1),
        }
    }

    /// Read a Super I/O register
    pub unsafe fn read(&mut self, addr: u8) -> u8 {
        self.addr.write(addr);
        self.data.read()
    }

    /// Write a Super I/O register
    pub unsafe fn write(&mut self, addr: u8, data: u8) {
        self.addr.write(addr);
        self.data.write(data);
    }
}
