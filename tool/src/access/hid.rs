use hidapi::HidDevice;

use crate::{
    Access,
    Error,
};

/// Use USB HID access, only for USB ECs
pub struct AccessHid {
    device: HidDevice,
    retries: u32,
    timeout: i32,
}

impl AccessHid {
    /// Use hidapi device with specified retries and timeout for each try in milliseconds
    pub fn new(device: HidDevice, retries: u32, timeout: i32) -> Result<Self, Error> {
        //TODO: probe?
        Ok(Self {
            device,
            retries,
            timeout,
        })
    }

    pub fn device(&mut self) -> &mut HidDevice {
        &mut self.device
    }

    unsafe fn command_try(&mut self, cmd: u8, data: &mut [u8]) -> Result<Option<u8>, Error> {
        const HID_CMD: usize = 1;
        const HID_RES: usize = 2;
        const HID_DATA: usize = 3;

        let mut hid_data = [0; 33];
        if data.len() + HID_DATA > hid_data.len() {
            return Err(Error::DataLength(data.len()));
        }

        hid_data[HID_CMD] = cmd;
        for i in 0..data.len() {
            hid_data[HID_DATA + i] = data[i];
        }

        let count = self.device.write(&hid_data)?;
        if count != hid_data.len() {
            return Err(Error::Verify);
        }

        let count = self.device.read_timeout(&mut hid_data[1..], self.timeout)?;
        if count == hid_data.len() - 1 {
            for i in 0..data.len() {
                data[i] = hid_data[HID_DATA + i];
            }

            Ok(Some(hid_data[HID_RES]))
        } else if count == 0 {
            Ok(None)
        } else {
            Err(Error::Verify)
        }
    }
}

impl Access for AccessHid {
    unsafe fn command(&mut self, cmd: u8, data: &mut [u8]) -> Result<u8, Error> {
        for _ in 0..self.retries {
            match self.command_try(cmd, data)? {
                Some(some) => return Ok(some),
                None => continue,
            }
        }

        Err(Error::Timeout)
    }

    fn data_size(&self) -> usize {
        32 - 2
    }
}
