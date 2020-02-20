use hwio::{Io, Pio};

pub struct SuperIo {
    addr: Pio<u8>,
    data: Pio<u8>,
}

impl SuperIo {
    /// Create a new SuperIo. `base` identifies the address port. The data port
    /// will be `base + 1`
    pub fn new(base: u16) -> Self {
        Self {
            addr: Pio::new(base),
            data: Pio::new(base + 1),
        }
    }

    pub unsafe fn read(&mut self, addr: u8) -> u8 {
        self.addr.write(addr);
        self.data.read()
    }

    pub unsafe fn write(&mut self, addr: u8, data: u8) {
        self.addr.write(addr);
        self.data.write(data);
    }
}
