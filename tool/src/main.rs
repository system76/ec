// SPDX-License-Identifier: MIT

#![allow(clippy::uninlined_format_args)]

use clap::Parser;
use ectool::{
    Access,
    AccessHid,
    AccessLpcLinux,
    AccessLpcSim,
    Ec,
    Error,
    Firmware,
    SecurityState,
    StdTimeout,
    Spi,
    SpiRom,
    SpiTarget,
};
use hidapi::HidApi;
use std::{
    fs,
    process,
    str,
    time::Duration,
    thread,
};

unsafe fn console(ec: &mut Ec<Box<dyn Access>>) -> Result<(), Error> {
    //TODO: driver support for reading debug region?
    let access = ec.access();

    let mut head = access.read_debug(0)? as usize;
    loop {
        let tail = access.read_debug(0)? as usize;
        if tail == 0 || head == tail {
            thread::sleep(Duration::from_millis(1));
        } else {
            while head != tail {
                head += 1;
                if head >= 256 { head = 1; }
                let c = access.read_debug(head as u8)?;
                print!("{}", c as char);
            }
        }
    }
}

unsafe fn flash_read<S: Spi>(spi: &mut SpiRom<S, StdTimeout>, rom: &mut [u8], sector_size: usize) -> Result<(), Error> {
    let mut address = 0;
    while address < rom.len() {
        eprint!("\rSPI Read {}K", address / 1024);
        let next_address = address + sector_size;
        let count = spi.read_at(address as u32, &mut rom[address..next_address])?;
        if count != sector_size {
            eprintln!("\ncount {} did not match sector size {}", count, sector_size);
            return Err(Error::Verify);
        }
        address = next_address;
    }
    eprintln!("\rSPI Read {}K", address / 1024);
    Ok(())
}

unsafe fn flash_inner(ec: &mut Ec<Box<dyn Access>>, firmware: &Firmware, target: SpiTarget, scratch: bool) -> Result<(), Error> {
    let new_rom = firmware.data.to_vec();

    // XXX: Get flash size programatically?
    let rom_size = new_rom.len();
    if rom_size % 1024 != 0 {
        println!("ROM size of {} is not valid", rom_size);
        return Err(ectool::Error::Verify);
    }

    let mut spi_bus = ec.spi(target, scratch)?;
    let mut spi = SpiRom::new(
        &mut spi_bus,
        StdTimeout::new(Duration::new(1, 0))
    );
    let sector_size = spi.sector_size();

    let mut rom = vec![0xFF; rom_size];
    flash_read(&mut spi, &mut rom, sector_size)?;

    eprintln!("Saving ROM to backup.rom");
    fs::write("backup.rom", &rom).map_err(|_| Error::Verify)?;

    // Program chip, sector by sector
    //TODO: write signature last
    {
        let mut address = 0;
        while address < rom_size {
            eprint!("\rSPI Write {}K", address / 1024);

            let next_address = address + sector_size;

            let mut matches = true;
            let mut erased = true;
            let mut new_erased = true;
            for i in address..next_address {
                if rom[i] != new_rom[i] {
                    matches = false;
                }
                if rom[i] != 0xFF {
                    erased = false;
                }
                if new_rom[i] != 0xFF {
                    new_erased = false;
                }
            }

            if ! matches {
                if ! erased {
                    spi.erase_sector(address as u32)?;
                }
                if ! new_erased {
                    let count = spi.write_at(address as u32, &new_rom[address..next_address])?;
                    if count != sector_size {
                        eprintln!("\nWrite count {} did not match sector size {}", count, sector_size);
                        return Err(Error::Verify);
                    }
                }
            }

            address = next_address;
        }
        eprintln!("\rSPI Write {}K", address / 1024);

        // Verify chip write
        flash_read(&mut spi, &mut rom, sector_size)?;
        for i in 0..rom.len() {
            if rom[i] != new_rom[i] {
                eprintln!("Failed to program: {:X} is {:X} instead of {:X}", i, rom[i], new_rom[i]);
                return Err(Error::Verify);
            }
        }
    }

    eprintln!("Successfully programmed SPI ROM");

    Ok(())
}

unsafe fn flash(ec: &mut Ec<Box<dyn Access>>, path: &str, target: SpiTarget) -> Result<(), Error> {
    let scratch = true;

    //TODO: remove unwraps
    let firmware_data = fs::read(path).unwrap();
    let firmware = Firmware::new(&firmware_data).unwrap();
    println!("file board: {:?}", str::from_utf8(firmware.board));
    println!("file version: {:?}", str::from_utf8(firmware.version));

    let data_size = ec.access().data_size();

    {
        let mut data = vec![0; data_size];
        let size = ec.board(&mut data)?;

        let ec_board = &data[..size];
        println!("ec board: {:?}", str::from_utf8(ec_board));

        assert!(ec_board == firmware.board, "file board does not match ec board");
    }

    {
        let mut data = vec![0; data_size];
        let size = ec.version(&mut data)?;

        let ec_version = &data[..size];
        println!("ec version: {:?}", str::from_utf8(ec_version));
    }

    if let Ok(security) = ec.security_get() {
        if security != SecurityState::Unlock {
            return Err(Error::WriteLocked);
        }
    }

    if scratch {
        // Wait for any key releases
        eprintln!("Waiting 5 seconds for all keys to be released");
        thread::sleep(Duration::new(5, 0));
    }

    eprintln!("Sync");
    let _ = process::Command::new("sync").status();

    let res = flash_inner(ec, &firmware, target, scratch);
    eprintln!("Result: {:X?}", res);

    eprintln!("Sync");
    let _ = process::Command::new("sync").status();

    if scratch {
        eprintln!("System will shut off in 5 seconds");
        thread::sleep(Duration::new(5, 0));

        eprintln!("Sync");
        let _ = process::Command::new("sync").status();

        ec.reset()?;
    }

    res
}

unsafe fn info(ec: &mut Ec<Box<dyn Access>>) -> Result<(), Error> {
    let data_size = ec.access().data_size();

    {
        print!("board: ");
        let mut data = vec![0; data_size];
        let size = ec.board(&mut data)?;
        for &b in data[..size].iter() {
            print!("{}", b as char);
        }
        println!();
    }

    {
        print!("version: ");
        let mut data = vec![0; data_size];
        let size = ec.version(&mut data)?;
        for &b in data[..size].iter() {
            print!("{}", b as char);
        }
        println!();
    }

    Ok(())
}

unsafe fn matrix(ec: &mut Ec<Box<dyn Access>>) -> Result<(), Error> {
    let data_size = ec.access().data_size();

    let mut data = vec![0; data_size];
    ec.matrix_get(&mut data)?;
    #[allow(clippy::get_first)]
    let rows = *data.get(0).unwrap_or(&0);
    let cols = *data.get(1).unwrap_or(&0);
    let mut byte = 2;
    let mut bit = 0;
    for _row in 0..rows {
        for _col in 0..cols {
            if (data.get(byte).unwrap_or(&0) & (1 << bit)) != 0 {
                print!("#");
            } else {
                print!("-");
            }

            bit += 1;
            if bit >= 8 {
                byte += 1;
                bit = 0;
            }
        }
        println!();
    }

    Ok(())
}

unsafe fn print(ec: &mut Ec<Box<dyn Access>>, message: &[u8]) -> Result<(), Error> {
    ec.print(message)?;

    Ok(())
}

unsafe fn fan_get(ec: &mut Ec<Box<dyn Access>>, index: u8) -> Result<(), Error> {
    let duty = ec.fan_get(index)?;
    println!("{}", duty);

    Ok(())
}

unsafe fn fan_set(ec: &mut Ec<Box<dyn Access>>, index: u8, duty: u8) -> Result<(), Error> {
    ec.fan_set(index, duty)
}

unsafe fn fan_tach(ec: &mut Ec<Box<dyn Access>>, index: u8) -> Result<(), Error> {
    let tach = ec.fan_tach(index)?;
    println!("{}", tach);

    Ok(())
}

unsafe fn keymap_get(ec: &mut Ec<Box<dyn Access>>, layer: u8, output: u8, input: u8) -> Result<(), Error> {
    let value = ec.keymap_get(layer, output, input)?;
    println!("{:04X}", value);

    Ok(())
}

unsafe fn keymap_set(ec: &mut Ec<Box<dyn Access>>, layer: u8, output: u8, input: u8, value: u16) -> Result<(), Error> {
    ec.keymap_set(layer, output, input, value)
}

unsafe fn security_get(ec: &mut Ec<Box<dyn Access>>) -> Result<(), Error> {
    println!("{:?}", ec.security_get()?);

    Ok(())
}

unsafe fn security_set(ec: &mut Ec<Box<dyn Access>>, state: SecurityState) -> Result<(), Error> {
    ec.security_set(state)?;
    println!("Shut down the system for the security state to take effect");

    Ok(())
}

fn parse_color(s: &str) -> Result<(u8, u8, u8), String> {
    let r = u8::from_str_radix(&s[0..2], 16);
    let g = u8::from_str_radix(&s[2..4], 16);
    let b = u8::from_str_radix(&s[4..6], 16);
    match (r, g, b) {
        (Ok(r), Ok(g), Ok(b)) if s.len() == 6 => Ok((r, g, b)),
        _ => Err(format!("Invalid color '{}'", s)),
    }
}


#[derive(Parser)]
#[clap(rename_all = "snake_case")]
enum SubCommand {
    Console,
    Fan {
        index: u8,
        duty: Option<u8>,
    },
    FanTach {
        index: u8,
    },
    Flash {
        path: String,
    },
    FlashBackup {
        path: String,
    },
    Info,
    Keymap {
        layer: u8,
        output: u8,
        input: u8,
        value: Option<String>,
    },
    LedColor {
        index: u8,
        #[clap(value_parser = parse_color)]
        value: Option<String>,
    },
    LedValue {
        index: u8,
        value: Option<u8>,
    },
    LedMode {
        layer: u8,
        #[clap(requires = "speed")]
        mode: Option<u8>,
        speed: Option<u8>,
    },
    LedSave,
    Reset,
    Matrix,
    Print {
        #[clap(required = true)]
        message: Vec<String>,
    },
    SetNoInput {
        value: bool,
    },
    Security {
        state: Option<String>,
    },
}

#[derive(clap::ValueEnum, Clone)]
enum AccessMode {
    LpcLinux,
    LpcSim,
    Hid,
}

#[derive(Parser)]
#[clap(name = "system76_ectool")]
struct Args {
    #[clap(
        long = "access",
        value_enum,
        default_value = "lpc-linux",
    )]
    access: AccessMode,
    #[clap(subcommand)]
    subcommand: SubCommand,
}

fn main() {
    let args = Args::parse();

    let get_ec = || -> Result<_, Error> {
        unsafe {
            match args.access {
                AccessMode::LpcLinux => {
                    let access = AccessLpcLinux::new(Duration::new(1, 0))?;
                    Ok(Ec::new(access)?.into_dyn())
                },
                AccessMode::LpcSim => {
                    let access = AccessLpcSim::new(Duration::new(1, 0))?;
                    Ok(Ec::new(access)?.into_dyn())
                },
                AccessMode::Hid => {
                    let api = HidApi::new()?;
                    for info in api.device_list() {
                        #[allow(clippy::single_match)]
                        match (info.vendor_id(), info.product_id(), info.interface_number()) {
                            // System76 launch_1
                            (0x3384, 0x0001, 1) |
                            // System76 launch_lite_1
                            (0x3384, 0x0005, 1) |
                            // System76 launch_2
                            (0x3384, 0x0006, 1) |
                            // System76 launch_heavy_1
                            (0x3384, 0x0007, 1) |
                            // System76 thelio_io_2
                            (0x3384, 0x000B, 1) => {
                                let device = info.open_device(&api)?;
                                let access = AccessHid::new(device, 10, 100)?;
                                return Ok(Ec::new(access)?.into_dyn());
                            }
                            _ => {},
                        }
                    }
                    Err(hidapi::HidError::HidApiErrorEmpty.into())
                }
            }
        }
    };
    let mut ec = match get_ec() {
        Ok(ec) => ec,
        Err(err) => {
            eprintln!("failed to connect to EC: {:X?}", err);
            process::exit(1);
        }
    };

    match args.subcommand {
        SubCommand::Console => match unsafe { console(&mut ec) } {
            Ok(()) => (),
            Err(err) => {
                eprintln!("failed to read console: {:X?}", err);
                process::exit(1);
            },
        },
        SubCommand::Fan { index, duty } => {
            match duty {
                Some(duty) => match unsafe { fan_set(&mut ec, index, duty) } {
                    Ok(()) => (),
                    Err(err) => {
                        eprintln!("failed to set fan {} to {}: {:X?}", index, duty, err);
                        process::exit(1);
                    },
                },
                None => match unsafe { fan_get(&mut ec, index) } {
                    Ok(()) => (),
                    Err(err) => {
                        eprintln!("failed to get fan {}: {:X?}", index, err);
                        process::exit(1);
                    },
                },
            }
        },
        SubCommand::FanTach { index } => {
            match unsafe { fan_tach(&mut ec, index) } {
                Ok(()) => (),
                Err(err) => {
                    eprintln!("failed to get fan {} tachometer: {:X?}", index, err);
                    process::exit(1);
                },
            }
        },
        SubCommand::Flash { path } => {
            match unsafe { flash(&mut ec, &path, SpiTarget::Main) } {
                Ok(()) => (),
                Err(err) => {
                    eprintln!("failed to flash '{}': {:X?}", path, err);
                    process::exit(1);
                },
            }
        },
        SubCommand::FlashBackup { path } => {
            match unsafe { flash(&mut ec, &path, SpiTarget::Backup) } {
                Ok(()) => (),
                Err(err) => {
                    eprintln!("failed to flash '{}': {:X?}", path, err);
                    process::exit(1);
                },
            }
        },
        SubCommand::Info => match unsafe { info(&mut ec) } {
            Ok(()) => (),
            Err(err) => {
                eprintln!("failed to read info: {:X?}", err);
                process::exit(1);
            },
        },
        SubCommand::Keymap { layer, output, input, value } => {
            match value {
                Some(value_str) => match u16::from_str_radix(value_str.trim_start_matches("0x"), 16) {
                    Ok(value) => match unsafe { keymap_set(&mut ec, layer, output, input, value) } {
                        Ok(()) => (),
                        Err(err) => {
                            eprintln!("failed to set keymap {}, {}, {} to {}: {:X?}", layer, output, input, value, err);
                            process::exit(1);
                        },
                    },
                    Err(err) => {
                        eprintln!("failed to parse value: '{}': {}", value_str, err);
                        process::exit(1);
                    }
                },
                None => match unsafe { keymap_get(&mut ec, layer, output, input) } {
                    Ok(()) => (),
                    Err(err) => {
                        eprintln!("failed to get keymap {}, {}, {}: {:X?}", layer, output, input, err);
                        process::exit(1);
                    },
                },
            }
        },
        SubCommand::LedColor { index, value } => {
            if let Some(value) = value {
                let (r, g, b) = parse_color(&value).unwrap();
                match unsafe { ec.led_set_color(index, r, g, b) } {
                    Ok(()) => (),
                    Err(err) => {
                        eprintln!("failed to set color {}: {:X?}", value, err);
                        process::exit(1);
                    },
                }
            } else {
                match unsafe { ec.led_get_color(index) } {
                    Ok((r, g, b)) => println!("{:02x}{:02x}{:02x}", r, g, b),
                    Err(err) => {
                        eprintln!("failed to get color: {:X?}", err);
                        process::exit(1);
                    },
                }
            }
        },
        SubCommand::LedValue { index, value } => {
            if let Some(value) = value {
                match unsafe { ec.led_set_value(index, value) } {
                    Ok(()) => (),
                    Err(err) => {
                        eprintln!("failed to set value {}: {:X?}", value, err);
                        process::exit(1);
                    },
                }
            } else {
                match unsafe { ec.led_get_value(index) } {
                    Ok((value, max)) => {
                        println!("value: {}", value);
                        println!("max: {}", max);
                    },
                    Err(err) => {
                        eprintln!("failed to get value: {:X?}", err);
                        process::exit(1);
                    },
                }
            }
        },
        SubCommand::LedMode { layer, mode, speed } => {
            if let (Some(mode), Some(speed)) = (mode, speed) {
                match unsafe { ec.led_set_mode(layer, mode, speed) } {
                    Ok(()) => (),
                    Err(err) => {
                        eprintln!("failed to set layer {} mode {} at speed {}: {:X?}", layer, mode, speed, err);
                        process::exit(1);
                    },
                }
            } else {
                match unsafe { ec.led_get_mode(layer) } {
                    Ok((mode, speed)) => {
                        println!("mode: {}", mode);
                        println!("speed: {}", speed);
                    },
                    Err(err) => {
                        eprintln!("failed to get mode for layer {}: {:X?}", layer, err);
                        process::exit(1);
                    },
                }
            }
        },
        SubCommand::LedSave => match unsafe { ec.led_save() } {
            Ok(()) => (),
            Err(err) => {
                eprintln!("failed to save LED settings: {:X?}", err);
                process::exit(1);
            },
        },
        SubCommand::Reset => match unsafe { ec.reset() } {
            Ok(()) => (),
            Err(err) => {
                eprintln!("failed to reset device: {:X?}", err);
                process::exit(1);
            },
        },
        SubCommand::Matrix => match unsafe { matrix(&mut ec) } {
            Ok(()) => (),
            Err(err) => {
                eprintln!("failed to read matrix: {:X?}", err);
                process::exit(1);
            },
        },
        SubCommand::Print { message } => for arg in message {
            let mut arg = arg.to_owned();
            arg.push('\n');
            match unsafe { print(&mut ec, arg.as_bytes()) } {
                Ok(()) => (),
                Err(err) => {
                    eprintln!("failed to print '{}': {:X?}", arg, err);
                    process::exit(1);
                },
            }
        },
        SubCommand::SetNoInput { value } => {
            match unsafe { ec.set_no_input(value) } {
                Ok(()) => (),
                Err(err) => {
                    eprintln!("failed to set no_input mode: {:X?}", err);
                    process::exit(1);
                }
            }
        },
        SubCommand::Security { state } => {
            match state {
                Some(value) => {
                    let state = match value.as_str() {
                        "lock" => SecurityState::PrepareLock,
                        "unlock" => SecurityState::PrepareUnlock,
                        _ => {
                            eprintln!("invalid security state '{}': must be 'lock' or 'unlock'", value);
                            process::exit(1);
                        }
                    };
                    match unsafe { security_set(&mut ec, state) } {
                        Ok(()) => (),
                        Err(err) => {
                            eprintln!("failed to set security state to '{}': {:X?}", value, err);
                            process::exit(1);
                        },
                    }
                },
                None => match unsafe { security_get(&mut ec) } {
                    Ok(()) => (),
                    Err(err) => {
                        eprintln!("failed to get security state: {:X?}", err);
                        process::exit(1);
                    },
                },
            }
        },
    }
}
