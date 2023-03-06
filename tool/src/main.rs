// SPDX-License-Identifier: MIT

use clap::{Arg, App, AppSettings, SubCommand};
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
    let rom_size = 128 * 1024;

    let mut new_rom = firmware.data.to_vec();
    while new_rom.len() < rom_size {
        new_rom.push(0xFF);
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

fn main() {
    let matches = App::new("system76_ectool")
        .setting(AppSettings::SubcommandRequired)
        .arg(Arg::with_name("access")
            .long("access")
            .possible_values(&["lpc-linux", "lpc-sim", "hid"])
            .default_value("lpc-linux")
        )
        .subcommand(SubCommand::with_name("console"))
        .subcommand(SubCommand::with_name("fan")
            .arg(Arg::with_name("index")
                .value_parser(clap::value_parser!(u8))
                .required(true)
            )
            .arg(Arg::with_name("duty")
                .value_parser(clap::value_parser!(u8))
            )
        )
        .subcommand(SubCommand::with_name("flash")
            .arg(Arg::with_name("path")
                .required(true)
            )
        )
        .subcommand(SubCommand::with_name("flash_backup")
            .arg(Arg::with_name("path")
                .required(true)
            )
        )
        .subcommand(SubCommand::with_name("info"))
        .subcommand(SubCommand::with_name("keymap")
            .arg(Arg::with_name("layer")
                .value_parser(clap::value_parser!(u8))
                .required(true)
            )
            .arg(Arg::with_name("output")
                .value_parser(clap::value_parser!(u8))
                .required(true)
            )
            .arg(Arg::with_name("input")
                .value_parser(clap::value_parser!(u8))
                .required(true)
            )
            .arg(Arg::with_name("value"))
        )
        .subcommand(SubCommand::with_name("led_color")
            .arg(Arg::with_name("index")
                .value_parser(clap::value_parser!(u8))
                .required(true)
            )
            .arg(Arg::with_name("value")
                .value_parser(parse_color)
            )
        )
        .subcommand(SubCommand::with_name("led_value")
            .arg(Arg::with_name("index")
                .value_parser(clap::value_parser!(u8))
                .required(true)
            )
            .arg(Arg::with_name("value")
                .value_parser(clap::value_parser!(u8))
            )
        )
        .subcommand(SubCommand::with_name("led_mode")
            .arg(Arg::with_name("layer")
                .value_parser(clap::value_parser!(u8))
                .required(true)
            )
            .arg(Arg::with_name("mode")
                .value_parser(clap::value_parser!(u8))
                .requires("speed")
            )
            .arg(Arg::with_name("speed")
                .value_parser(clap::value_parser!(u8))
            )
        )
        .subcommand(SubCommand::with_name("led_save"))
        .subcommand(SubCommand::with_name("matrix"))
        .subcommand(SubCommand::with_name("print")
            .arg(Arg::with_name("message")
                .required(true)
                .multiple(true)
            )
        )
        .subcommand(SubCommand::with_name("set_no_input")
            .arg(Arg::with_name("value")
                .possible_values(&["true", "false"])
                .required(true)
            )
        )
        .subcommand(SubCommand::with_name("security")
            .arg(Arg::with_name("state")
                .possible_values(&["lock", "unlock"])
            )
        )
        .get_matches();

    let get_ec = || -> Result<_, Error> {
        unsafe {
            match matches.value_of("access").unwrap() {
                "lpc-linux" => {
                    let access = AccessLpcLinux::new(Duration::new(1, 0))?;
                    Ok(Ec::new(access)?.into_dyn())
                },
                "lpc-sim" => {
                    let access = AccessLpcSim::new(Duration::new(1, 0))?;
                    Ok(Ec::new(access)?.into_dyn())
                },
                "hid" => {
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
                            (0x3384, 0x0007, 1) => {
                                let device = info.open_device(&api)?;
                                let access = AccessHid::new(device, 10, 100)?;
                                return Ok(Ec::new(access)?.into_dyn());
                            }
                            _ => {},
                        }
                    }
                    Err(hidapi::HidError::OpenHidDeviceError.into())
                }
                _ => unreachable!(),
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

    match matches.subcommand() {
        Some(("console", _sub_m)) => match unsafe { console(&mut ec) } {
            Ok(()) => (),
            Err(err) => {
                eprintln!("failed to read console: {:X?}", err);
                process::exit(1);
            },
        },
        Some(("fan", sub_m)) => {
            let index = sub_m.value_of("index").unwrap().parse::<u8>().unwrap();
            let duty_opt = sub_m.value_of("duty").map(|x| x.parse::<u8>().unwrap());
            match duty_opt {
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
        Some(("flash", sub_m)) => {
            let path = sub_m.value_of("path").unwrap();
            match unsafe { flash(&mut ec, path, SpiTarget::Main) } {
                Ok(()) => (),
                Err(err) => {
                    eprintln!("failed to flash '{}': {:X?}", path, err);
                    process::exit(1);
                },
            }
        },
        Some(("flash_backup", sub_m)) => {
            let path = sub_m.value_of("path").unwrap();
            match unsafe { flash(&mut ec, path, SpiTarget::Backup) } {
                Ok(()) => (),
                Err(err) => {
                    eprintln!("failed to flash '{}': {:X?}", path, err);
                    process::exit(1);
                },
            }
        },
        Some(("info", _sub_m)) => match unsafe { info(&mut ec) } {
            Ok(()) => (),
            Err(err) => {
                eprintln!("failed to read info: {:X?}", err);
                process::exit(1);
            },
        },
        Some(("keymap", sub_m)) => {
            let layer = sub_m.value_of("layer").unwrap().parse::<u8>().unwrap();
            let output = sub_m.value_of("output").unwrap().parse::<u8>().unwrap();
            let input = sub_m.value_of("input").unwrap().parse::<u8>().unwrap();
            match sub_m.value_of("value") {
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
        Some(("led_color", sub_m)) => {
            let index = sub_m.value_of("index").unwrap().parse::<u8>().unwrap();
            let value = sub_m.value_of("value");
            if let Some(value) = value {
                let (r, g, b) = parse_color(value).unwrap();
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
        Some(("led_value", sub_m)) => {
            let index = sub_m.value_of("index").unwrap().parse::<u8>().unwrap();
            let value = sub_m.value_of("value").map(|x| x.parse::<u8>().unwrap());
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
        Some(("led_mode", sub_m)) => {
            let layer = sub_m.value_of("layer").unwrap().parse::<u8>().unwrap();
            let mode = sub_m.value_of("mode").map(|x| x.parse::<u8>().unwrap());
            let speed = sub_m.value_of("speed").map(|x| x.parse::<u8>().unwrap());
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
        Some(("led_save", _sub_m)) => match unsafe { ec.led_save() } {
            Ok(()) => (),
            Err(err) => {
                eprintln!("failed to save LED settings: {:X?}", err);
                process::exit(1);
            },
        },
        Some(("matrix", _sub_m)) => match unsafe { matrix(&mut ec) } {
            Ok(()) => (),
            Err(err) => {
                eprintln!("failed to read matrix: {:X?}", err);
                process::exit(1);
            },
        },
        Some(("print", sub_m)) => for arg in sub_m.values_of("message").unwrap() {
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
        Some(("set_no_input", sub_m)) => {
            let no_input = sub_m.value_of("value").unwrap().parse::<bool>().unwrap();
            match unsafe { ec.set_no_input(no_input) } {
                Ok(()) => (),
                Err(err) => {
                    eprintln!("failed to set no_input mode: {:X?}", err);
                    process::exit(1);
                }
            }
        },
        Some(("security", sub_m)) => {
            match sub_m.value_of("state") {
                Some(value) => {
                    let state = match value {
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
        _ => unreachable!()
    }
}
