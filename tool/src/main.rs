use ectool::{
    Access,
    AccessLpcLinux,
    Ec,
    Error,
    Firmware,
    StdTimeout,
    Spi,
    SpiRom,
    SpiTarget,
};
use std::{
    env,
    fs,
    process,
    str::{self, FromStr},
    time::Duration,
    thread,
};

unsafe fn ec() -> Result<Ec<AccessLpcLinux>, Error> {
    let access = AccessLpcLinux::new(Duration::new(1, 0))?;
    Ec::new(access)
}

unsafe fn console() -> Result<(), Error> {
    //TODO: driver support for reading debug region?
    let mut ec = ec()?;
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

unsafe fn flash_inner(ec: &mut Ec<AccessLpcLinux>, firmware: &Firmware, target: SpiTarget, scratch: bool) -> Result<(), Error> {
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

unsafe fn flash(path: &str, target: SpiTarget) -> Result<(), Error> {
    let scratch = true;

    //TODO: remove unwraps
    let firmware_data = fs::read(path).unwrap();
    let firmware = Firmware::new(&firmware_data).unwrap();
    println!("file board: {:?}", str::from_utf8(firmware.board));
    println!("file version: {:?}", str::from_utf8(firmware.version));

    let mut ec = ec()?;
    let data_size = ec.access().data_size();

    {
        let mut data = vec![0; data_size];
        let size = ec.board(&mut data)?;

        let ec_board = &data[..size];
        println!("ec board: {:?}", str::from_utf8(ec_board));

        if ec_board != firmware.board {
            panic!("file board does not match ec board");
        }
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

    let res = flash_inner(&mut ec, &firmware, target, scratch);
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

unsafe fn info() -> Result<(), Error> {
    let mut ec = ec()?;
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

unsafe fn print(message: &[u8]) -> Result<(), Error> {
    let mut ec = ec()?;

    ec.print(message)?;

    Ok(())
}

unsafe fn fan_get(index: u8) -> Result<(), Error> {
    let mut ec = ec()?;

    let duty = ec.fan_get(index)?;
    println!("{}", duty);

    Ok(())
}

unsafe fn fan_set(index: u8, duty: u8) -> Result<(), Error> {
    let mut ec = ec()?;

    ec.fan_set(index, duty)
}

unsafe fn keymap_get(layer: u8, output: u8, input: u8) -> Result<(), Error> {
    let mut ec = ec()?;

    let value = ec.keymap_get(layer, output, input)?;
    println!("{:04X}", value);

    Ok(())
}

unsafe fn keymap_set(layer: u8, output: u8, input: u8, value: u16) -> Result<(), Error> {
    let mut ec = ec()?;

    ec.keymap_set(layer, output, input, value)
}

fn usage() {
    eprintln!("  console");
    eprintln!("  flash [file]");
    eprintln!("  flash_backup [file]");
    eprintln!("  fan [index] <duty>");
    eprintln!("  info");
    eprintln!("  keymap [layer] [output] [input] <value>");
    eprintln!("  print [message]");
}

fn parse_arg_opt<I: Iterator<Item=String>, F: FromStr>(args: &mut I, name: &str) -> Option<F> {
    match args.next() {
        Some(arg) => match arg.parse::<F>() {
            Ok(ok) => Some(ok),
            Err(_err) => {
                eprintln!("failed to parse {}: '{}'", name, arg);
                process::exit(1);
            },
        },
        None  => None,
    }
}

fn parse_arg<I: Iterator<Item=String>, F: FromStr>(args: &mut I, name: &str) -> F {
    match parse_arg_opt(args, name) {
        Some(some) => some,
        None => {
            eprintln!("no {} provided", name);
            process::exit(1);
        }
    }
}

fn main() {
    let mut args = env::args().skip(1);
    match args.next() {
        Some(arg) => match arg.as_str() {
            "console" => match unsafe { console() } {
                Ok(()) => (),
                Err(err) => {
                    eprintln!("failed to read console: {:X?}", err);
                    process::exit(1);
                },
            },
            "fan" => {
                let index = parse_arg(&mut args, "index");
                let duty_opt = parse_arg_opt(&mut args, "duty");
                match duty_opt {
                    Some(duty) => match unsafe { fan_set(index, duty) } {
                        Ok(()) => (),
                        Err(err) => {
                            eprintln!("failed to set fan {} to {}: {:X?}", index, duty, err);
                            process::exit(1);
                        },
                    },
                    None => match unsafe { fan_get(index) } {
                        Ok(()) => (),
                        Err(err) => {
                            eprintln!("failed to get fan {}: {:X?}", index, err);
                            process::exit(1);
                        },
                    },
                }
            },
            "flash" => match args.next() {
                Some(path) => match unsafe { flash(&path, SpiTarget::Main) } {
                    Ok(()) => (),
                    Err(err) => {
                        eprintln!("failed to flash '{}': {:X?}", path, err);
                        process::exit(1);
                    },
                },
                None => {
                    eprintln!("no file provided");
                    process::exit(1);
                }
            },
            "flash_backup" => match args.next() {
                Some(path) => match unsafe { flash(&path, SpiTarget::Backup) } {
                    Ok(()) => (),
                    Err(err) => {
                        eprintln!("failed to flash '{}': {:X?}", path, err);
                        process::exit(1);
                    },
                },
                None => {
                    eprintln!("no file provided");
                    process::exit(1);
                }
            },
            "info" => match unsafe { info() } {
                Ok(()) => (),
                Err(err) => {
                    eprintln!("failed to read info: {:X?}", err);
                    process::exit(1);
                },
            },
            "keymap" => {
                let layer = parse_arg(&mut args, "layer");
                let output = parse_arg(&mut args, "output");
                let input = parse_arg(&mut args, "input");
                match args.next() {
                    Some(value_str) => match u16::from_str_radix(value_str.trim_start_matches("0x"), 16) {
                        Ok(value) => match unsafe { keymap_set(layer, output, input, value) } {
                            Ok(()) => (),
                            Err(err) => {
                                eprintln!("failed to set keymap {}, {}, {} to {}: {:X?}", layer, output, input, value, err);
                                process::exit(1);
                            },
                        },
                        Err(err) => {
                            eprintln!("failed to parse value: '{}': {}", arg, err);
                            process::exit(1);
                        }
                    },
                    None => match unsafe { keymap_get(layer, output, input) } {
                        Ok(()) => (),
                        Err(err) => {
                            eprintln!("failed to get keymap {}, {}, {}: {:X?}", layer, output, input, err);
                            process::exit(1);
                        },
                    },
                }
            },
            "print" => for mut arg in args {
                arg.push('\n');
                match unsafe { print(&arg.as_bytes()) } {
                    Ok(()) => (),
                    Err(err) => {
                        eprintln!("failed to print '{}': {:X?}", arg, err);
                        process::exit(1);
                    },
                }
            },
            _ => {
                eprintln!("unknown subcommand '{}'", arg);
                usage();
                process::exit(1);
            },
        },
        None => {
            eprintln!("no subcommand provided");
            usage();
            process::exit(1);
        },
    }

}
