pub struct Firmware<'a> {
    pub board: &'a [u8],
    pub version: &'a [u8],
    pub data: &'a [u8],
}

fn firmware_str<'a>(data: &'a [u8], key: &[u8]) -> Option<&'a [u8]> {
    let mut data_i = 0;

    //First, locate the key
    let mut key_i = 0;
    while data_i < data.len() && key_i < key.len() {
        if data[data_i] == key[key_i] {
            key_i += 1;
        } else {
            key_i = 0;
        }
        data_i += 1;
    }

    // Return None if key not found
    if key_i < key.len() {
        return None;
    }

    // Locate end of data
    let start = data_i;
    while data_i < data.len() {
        if data[data_i] == 0 {
            break;
        }
        data_i += 1;
    }

    Some(&data[start..data_i])
}

impl<'a> Firmware<'a> {
    pub fn new(data: &'a [u8]) -> Option<Self> {
        let board = firmware_str(data, b"76EC_BOARD=")?;
        let version = firmware_str(data, b"76EC_VERSION=")?;
        Some(Self {
            data,
            board,
            version,
        })
    }
}
