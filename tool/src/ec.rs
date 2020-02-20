use hwio::{Io, Pio};

use crate::error::Error;
use crate::super_io::SuperIo;
use crate::timeout::Timeout;

#[derive(Clone, Copy, Debug)]
#[repr(u8)]
pub enum EcCmd {
    None = 0,
    Probe = 1,
    Board = 2,
    Version = 3,
}

pub struct Ec<T: Timeout> {
    cmd: u16,
    dbg: u16,
    timeout: T,
}

impl<T: Timeout> Ec<T> {
    /// Probes for a compatible EC
    pub unsafe fn new(primary: bool, timeout: T) -> Result<Self, Error> {
        let mut sio = SuperIo::new(if primary { 0x2E } else { 0x4E });

        let id =
            (sio.read(0x20) as u16) << 8 |
            (sio.read(0x21) as u16);

        match id {
            0x5570 | 0x8587 => (),
            _ => return Err(Error::SuperIoId(id)),
        }

        let mut ec = Ec {
            cmd: if primary { 0xC00 } else { 0xE00 },
            dbg: if primary { 0xD00 } else { 0xF00 },
            timeout,
        };

        ec.probe()?;

        Ok(ec)
    }

    /// Read from the command space
    pub unsafe fn read(&mut self, addr: u8) -> u8 {
        Pio::<u8>::new(
            self.cmd + (addr as u16)
        ).read()
    }

    /// Write to the command space
    pub unsafe fn write(&mut self, addr: u8, data: u8) {
        Pio::<u8>::new(
            self.cmd + (addr as u16)
        ).write(data)
    }

    /// Read from the debug space
    pub unsafe fn debug(&mut self, addr: u8) -> u8 {
        Pio::<u8>::new(
            self.dbg + (addr as u16)
        ).read()
    }

    /// Returns true if a command can be sent
    pub unsafe fn can_command(&mut self) -> bool {
        self.read(0) == EcCmd::None as u8
    }

    /// Start an EC command
    pub unsafe fn command_start(&mut self, cmd: EcCmd) -> Result<(), Error> {
        if self.can_command() {
            self.write(0, cmd as u8);
            Ok(())
        } else {
            Err(Error::WouldBlock)
        }
    }

    /// Finish an EC command
    pub unsafe fn command_finish(&mut self) -> Result<(), Error> {
        if self.can_command() {
            Err(Error::WouldBlock)
        } else {
            match self.read(1) {
                0 => Ok(()),
                err => Err(Error::Protocol(err)),
            }
        }
    }

    /// Run an EC command (start and finish)
    pub unsafe fn command(&mut self, cmd: EcCmd) -> Result<(), Error> {
        self.timeout.reset();

        while self.timeout.running() {
            match self.command_start(cmd) {
                Ok(()) => break,
                Err(err) => match err {
                    Error::WouldBlock => (),
                    _ => return Err(err)
                },
            }
        }

        while self.timeout.running() {
            match self.command_finish() {
                Ok(ok) => return Ok(ok),
                Err(err) => match err {
                    Error::WouldBlock => (),
                    _ => return Err(err)
                },
            }
        }

        Err(Error::Timeout)
    }

    /// Probe for EC
    pub unsafe fn probe(&mut self) -> Result<u8, Error> {
        self.command(EcCmd::Probe)?;
        let signature = (
            self.read(2),
            self.read(3)
        );
        if signature == (0x76, 0xEC) {
            let version = self.read(4);
            Ok(version)
        } else {
            Err(Error::Signature(signature))
        }
    }

    /// Read board from EC
    pub unsafe fn board(&mut self, data: &mut [u8]) -> Result<usize, Error> {
        self.command(EcCmd::Board)?;
        let mut i = 0;
        while i < data.len() && (i + 2) < 256 {
            data[i] = self.read((i + 2) as u8);
            if data[i] == 0 {
                break;
            }
            i += 1;
        }
        Ok(i)
    }

    /// Read version from EC
    pub unsafe fn version(&mut self, data: &mut [u8]) -> Result<usize, Error> {
        self.command(EcCmd::Version)?;
        let mut i = 0;
        while i < data.len() && (i + 2) < 256 {
            data[i] = self.read((i + 2) as u8);
            if data[i] == 0 {
                break;
            }
            i += 1;
        }
        Ok(i)
    }
}
