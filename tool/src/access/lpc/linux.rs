// SPDX-License-Identifier: MIT

use std::{
    fs,
    io::{
        self,
        Read,
        Write,
        Seek,
        SeekFrom,
    },
    os::unix::io::AsRawFd,
    path::Path,
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

struct PortLock {
    start: u16,
    len: u16,
    file: fs::File,
}

impl PortLock {
    pub fn new(start: u16, end: u16) -> io::Result<Self> {
        if end < start {
            return Err(io::Error::new(
                io::ErrorKind::InvalidInput,
                "PortLock::new: end < start"
            ));
        }
        let len = (end - start) + 1;

        let file = fs::OpenOptions::new()
            .read(true)
            .write(true)
            .open("/dev/port")?;

        let mut flock = libc::flock {
            l_type: libc::F_WRLCK as _,
            l_whence: libc::SEEK_SET as _,
            l_start: start as _,
            l_len: len as _,
            l_pid: 0,
        };

        if unsafe { libc::fcntl(file.as_raw_fd(), libc::F_SETLK, &mut flock) < 0 } {
            return Err(io::Error::last_os_error());
        }

        Ok(Self {
            start,
            len,
            file,
        })
    }

    fn seek(&mut self, offset: u16) -> io::Result<()> {
        if offset >= self.len {
            return Err(io::Error::new(
                io::ErrorKind::InvalidInput,
                "PortLock::seek: offset >= len"
            ));
        }
        let port = self.start + offset;
        let pos = self.file.seek(SeekFrom::Start(port as u64))?;
        if pos != port as u64 {
            return Err(io::Error::new(
                io::ErrorKind::UnexpectedEof,
                "PortLock::seek: failed to seek to port"
            ));
        }
        Ok(())
    }

    pub fn read(&mut self, offset: u16) -> io::Result<u8> {
        self.seek(offset)?;
        let mut data = [0];
        self.file.read_exact(&mut data)?;
        Ok(data[0])
    }

    pub fn write(&mut self, offset: u16, value: u8) -> io::Result<()> {
        self.seek(offset)?;
        self.file.write_all(&[value])
    }
}

/// Use /dev/port access with file locking
pub struct AccessLpcLinux {
    cmd: PortLock,
    dbg: PortLock,
    timeout: StdTimeout,
}

impl AccessLpcLinux {
    /// Locks ports and then returns access object
    pub unsafe fn new(timeout: Duration) -> Result<Self, Error> {
        // TODO: is there a better way to probe before running a command?
        if ! Path::new("/sys/bus/acpi/devices/17761776:00").is_dir() &&
           ! Path::new("/sys/bus/acpi/devices/DASHEC00:00").is_dir() &&
           ! Path::new("/sys/bus/acpi/devices/DSHR4543:00").is_dir() {
            return Err(Error::Io(io::Error::new(
                io::ErrorKind::NotFound,
                "Failed to find Dasharo ACPI device",
            )));
        }

        let cmd = PortLock::new(SMFI_CMD_BASE, SMFI_CMD_BASE + SMFI_CMD_SIZE as u16 - 1)?;
        let dbg = PortLock::new(SMFI_DBG_BASE, SMFI_DBG_BASE + SMFI_DBG_SIZE as u16 - 1)?;
        Ok(Self {
            cmd,
            dbg,
            timeout: StdTimeout::new(timeout),
        })
    }

    /// Read from the command space
    unsafe fn read_cmd(&mut self, addr: u8) -> Result<u8, Error> {
        Ok(self.cmd.read(addr as u16)?)
    }

    /// Write to the command space
    unsafe fn write_cmd(&mut self, addr: u8, data: u8) -> Result<(), Error> {
        Ok(self.cmd.write(addr as u16, data)?)
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

impl Access for AccessLpcLinux {
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
        Ok(self.dbg.read(addr as u16)?)
    }
}
