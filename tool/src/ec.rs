use crate::{
    Access,
    Error,
    Spi,
    SpiTarget,
};

#[derive(Clone, Copy, Debug)]
#[repr(u8)]
enum Cmd {
    // None = 0,
    Probe = 1,
    Board = 2,
    Version = 3,
    Print = 4,
    Spi = 5,
    Reset = 6,
    FanGet = 7,
    FanSet = 8,
    KeymapGet = 9,
    KeymapSet = 10,
}

const CMD_SPI_FLAG_READ: u8 = 1 << 0;
const CMD_SPI_FLAG_DISABLE: u8 = 1 << 1;
const CMD_SPI_FLAG_SCRATCH: u8 = 1 << 2;
const CMD_SPI_FLAG_BACKUP: u8 = 1 << 3;

/// Run EC commands using a provided access method
pub struct Ec<A: Access> {
    access: A,
    version: u8,
}

impl<A: Access> Ec<A> {
    /// Probes for a compatible EC
    pub unsafe fn new(access: A) -> Result<Self, Error> {
        // Create EC struct with provided access method and timeout
        let mut ec = Ec {
            access,
            version: 0,
        };

        // Read version of protocol
        ec.version = ec.probe()?;

        // Make sure protocol version is supported
        match ec.version {
            1 => (),
            _ => return Err(Error::Version(ec.version)),
        }

        Ok(ec)
    }

    /// Unsafe access to access
    pub unsafe fn access(&mut self) -> &mut A {
        &mut self.access
    }

    unsafe fn command(&mut self, cmd: Cmd, data: &mut [u8]) -> Result<(), Error> {
        match self.access.command(cmd as u8, data)? {
            0 => Ok(()),
            err => Err(Error::Protocol(err)),
        }
    }

    /// Probe for EC
    pub unsafe fn probe(&mut self) -> Result<u8, Error> {
        let mut data = [0; 3];
        self.command(Cmd::Probe, &mut data)?;
        let signature = (data[0], data[1]);
        if signature == (0x76, 0xEC) {
            let version = data[2];
            Ok(version)
        } else {
            Err(Error::Signature(signature))
        }
    }

    /// Read board from EC
    pub unsafe fn board(&mut self, data: &mut [u8]) -> Result<usize, Error> {
        self.command(Cmd::Board, data)?;
        let mut i = 0;
        while i < data.len() {
            if data[i] == 0 {
                break;
            }
            i += 1;
        }
        Ok(i)
    }

    /// Read version from EC
    pub unsafe fn version(&mut self, data: &mut [u8]) -> Result<usize, Error> {
        self.command(Cmd::Version, data)?;
        let mut i = 0;
        while i < data.len() {
            if data[i] == 0 {
                break;
            }
            i += 1;
        }
        Ok(i)
    }

    /// Print data to EC console
    pub unsafe fn print(&mut self, data: &[u8]) -> Result<usize, Error> {
        //TODO: use self.access.data_size()
        let flags = 0;
        for chunk in data.chunks(256 - 4) {
            let mut data = [0; 256 - 2];
            data[0] = flags;
            data[1] = chunk.len() as u8;
            for i in 0..chunk.len() {
                data[i + 2] = chunk[i];
            }
            self.command(Cmd::Print, &mut data)?;
            if data[1] != chunk.len() as u8 {
                return Err(Error::Verify);
            }
        }
        Ok(data.len())
    }

    /// Access EC SPI bus
    pub unsafe fn spi(&mut self, target: SpiTarget, scratch: bool) -> Result<EcSpi<A>, Error> {
        let mut spi = EcSpi {
            ec: self,
            target,
            scratch,
        };
        spi.reset()?;
        Ok(spi)
    }

    /// Reset EC. Will also power off computer.
    pub unsafe fn reset(&mut self) -> Result<(), Error> {
        self.command(Cmd::Reset, &mut [])
    }

    /// Read fan duty cycle by fan index
    pub unsafe fn fan_get(&mut self, index: u8) -> Result<u8, Error> {
        let mut data = [
            index,
            0
        ];
        self.command(Cmd::FanGet, &mut data)?;
        Ok(data[1])
    }

    /// Set fan duty cycle by fan index
    pub unsafe fn fan_set(&mut self, index: u8, duty: u8) -> Result<(), Error> {
        let mut data = [
            index,
            duty
        ];
        self.command(Cmd::FanSet, &mut data)
    }

    /// Read keymap data by layout, output pin, and input pin
    pub unsafe fn keymap_get(&mut self, layer: u8, output: u8, input: u8) -> Result<u16, Error> {
        let mut data = [
            layer,
            output,
            input,
            0,
            0
        ];
        self.command(Cmd::KeymapGet, &mut data)?;
        Ok(
            (data[3] as u16) |
            ((data[4] as u16) << 8)
        )
    }

    /// Set keymap data by layout, output pin, and input pin
    pub unsafe fn keymap_set(&mut self, layer: u8, output: u8, input: u8, value: u16) -> Result<(), Error> {
        let mut data = [
            layer,
            output,
            input,
            value as u8,
            (value >> 8) as u8
        ];
        self.command(Cmd::KeymapSet, &mut data)
    }
}

pub struct EcSpi<'a, A: Access> {
    ec: &'a mut Ec<A>,
    target: SpiTarget,
    scratch: bool,
}

impl<'a, A: Access> EcSpi<'a, A> {
    fn flags(&self, read: bool, disable: bool) -> u8 {
        let mut flags = 0;

        if read {
            flags |= CMD_SPI_FLAG_READ;
        }

        if disable {
            flags |= CMD_SPI_FLAG_DISABLE;
        }

        if self.scratch {
            flags |= CMD_SPI_FLAG_SCRATCH;
        }

        match self.target {
            SpiTarget::Main => (),
            SpiTarget::Backup => {
                flags |= CMD_SPI_FLAG_BACKUP;
            },
        }

        flags
    }
}

impl<'a, A: Access> Spi for EcSpi<'a, A> {
    fn target(&self) -> SpiTarget {
        self.target
    }

    /// Disable SPI chip, must be done before and after a transaction
    unsafe fn reset(&mut self) -> Result<(), Error> {
        let flags = self.flags(false, true);
        let mut data = [
            flags,
            0,
        ];
        self.ec.command(Cmd::Spi, &mut data)?;
        if data[1] != 0 {
            return Err(Error::Verify);
        }
        Ok(())
    }

    /// SPI read
    unsafe fn read(&mut self, data: &mut [u8]) -> Result<usize, Error> {
        //TODO: use self.access.data_size()
        let flags = self.flags(true, false);
        for chunk in data.chunks_mut(256 - 4) {
            let mut data = [0; 256 - 2];
            data[0] = flags;
            data[1] = chunk.len() as u8;
            self.ec.command(Cmd::Spi, &mut data)?;
            if data[1] != chunk.len() as u8 {
                return Err(Error::Verify);
            }
            for i in 0..chunk.len() {
                chunk[i] = data[i + 2];
            }
        }
        Ok(data.len())
    }

    /// SPI write
    unsafe fn write(&mut self, data: &[u8]) -> Result<usize, Error> {
        //TODO: use self.access.data_size()
        let flags = self.flags(false, false);
        for chunk in data.chunks(256 - 4) {
            let mut data = [0; 256 - 2];
            data[0] = flags;
            data[1] = chunk.len() as u8;
            for i in 0..chunk.len() {
                data[i + 2] = chunk[i];
            }
            self.ec.command(Cmd::Spi, &mut data)?;
            if data[1] != chunk.len() as u8 {
                return Err(Error::Verify);
            }
        }
        Ok(data.len())
    }
}

impl<'a, A: Access> Drop for EcSpi<'a, A> {
    fn drop(&mut self) {
        unsafe {
            let _ = self.reset();
        }
    }
}
