use std::{
    io,
    net::UdpSocket,
    time::Duration,
};

use crate::{
    Access,
    Error,
    StdTimeout,
    Timeout,
    timeout,
};

use super::*;

pub struct AccessLpcSim {
    socket: UdpSocket,
    timeout: StdTimeout,
}

impl AccessLpcSim {
    pub unsafe fn new(timeout: Duration) -> Result<Self, Error> {
        let socket = UdpSocket::bind("127.0.0.1:0")?;
        socket.connect("127.0.0.1:8587")?;
        let mut access = Self {
            socket,
            timeout: StdTimeout::new(timeout),
        };
        access.transaction(0x00, 0, 0)?;
        Ok(access)
    }

    fn transaction(&mut self, kind: u8, addr: u16, value: u8) -> io::Result<u8> {
        let addr = addr.to_le_bytes();
        let request = [kind as u8, addr[0], addr[1], value];
        if self.socket.send(&request)? != request.len() {
            return Err(io::Error::new(
                io::ErrorKind::UnexpectedEof,
                "Socket request incorrect size"
            ));
        }
    
        let mut response = [0];
        if self.socket.recv(&mut response)? != response.len() {
            return Err(io::Error::new(
                io::ErrorKind::UnexpectedEof,
                "Socket response incorrect size"
            ));
        }
    
        Ok(response[0])
    }
    
    pub fn inb(&mut self, addr: u16) -> Result<u8, Error> {
        Ok(self.transaction(0x01, addr, 0)?)
    }
    
    pub fn outb(&mut self, addr: u16, value: u8) -> Result<(), Error> {
        self.transaction(0x02, addr, value)?;
        Ok(())
    }

    /// Read from the command space
    unsafe fn read_cmd(&mut self, addr: u8) -> Result<u8, Error> {
        self.inb(SMFI_CMD_BASE + u16::from(addr))
    }

    /// Write to the command space
    unsafe fn write_cmd(&mut self, addr: u8, data: u8) -> Result<(), Error> {
        self.outb(SMFI_CMD_BASE + u16::from(addr), data)
    }

    /// Returns Ok if a command can be sent
    unsafe fn command_check(&mut self) -> Result<(), Error> {
        if self.read_cmd(SMFI_CMD_CMD)? == 0 {
            Ok(())
        } else {
            Err(Error::WouldBlock)
        }
    }
}

impl Access for AccessLpcSim {
    unsafe fn command(&mut self, cmd: u8, data: &mut [u8]) -> Result<u8, Error> {
        // Test data length
        if data.len() > self.data_size() {
            return Err(Error::DataLength(data.len()));
        }

        // All previous commands should be finished
        self.command_check()?;

        // Write data bytes, index should be valid due to length test above
        for i in 0..data.len() {
            self.write_cmd(i as u8 + SMFI_CMD_DATA, data[i])?;
        }

        // Write command byte, which starts command
        self.write_cmd(SMFI_CMD_CMD, cmd as u8)?;

        // Wait for command to finish with timeout
        self.timeout.reset();
        timeout!(self.timeout, self.command_check())?;

        // Read data bytes, index should be valid due to length test above
        for i in 0..data.len() {
            data[i] = self.read_cmd(i as u8 + SMFI_CMD_DATA)?;
        }

        // Return response byte
        self.read_cmd(SMFI_CMD_RES)
    }

    fn data_size(&self) -> usize {
        SMFI_CMD_SIZE - SMFI_CMD_DATA as usize
    }

    unsafe fn read_debug(&mut self, addr: u8) -> Result<u8, Error> {
        self.inb(SMFI_DBG_BASE + u16::from(addr))
    }
}
