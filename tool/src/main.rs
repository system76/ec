use ectool::{
    Ec,
    Error,
    Firmware,
    Spi,
    SpiRom,
    SpiTarget,
    Timeout,
};
use std::{
    env,
    fs,
    io,
    process,
    str,
    time::{Duration, Instant},
    thread,
};

pub struct StdTimeout {
    instant: Instant,
    duration: Duration,
}

impl StdTimeout {
    pub fn new(duration: Duration) -> Self {
        StdTimeout {
            instant: Instant::now(),
            duration
        }
    }
}

impl Timeout for StdTimeout {
    fn reset(&mut self) {
        self.instant = Instant::now();
    }

    fn running(&self) -> bool {
        self.instant.elapsed() < self.duration
    }
}

unsafe fn iopl() {
    extern {
        fn iopl(level: isize) -> isize;
    }

    if iopl(3) < 0 {
        eprintln!("failed to get I/O permission: {}", io::Error::last_os_error());
        process::exit(1);
    }
}

unsafe fn console() -> Result<(), Error> {
    iopl();

    let mut ec = Ec::new(
        StdTimeout::new(Duration::new(1, 0)),
    )?;

    let mut head = ec.debug(0) as usize;
    loop {
        let tail = ec.debug(0) as usize;
        if tail == 0 || head == tail {
            thread::sleep(Duration::from_millis(1));
        } else {
            while head != tail {
                head += 1;
                if head >= 256 { head = 1; }
                let c = ec.debug(head as u8);
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

unsafe fn flash_inner(ec: &mut Ec<StdTimeout>, firmware: &Firmware, target: SpiTarget, scratch: bool) -> Result<(), Error> {
    let rom_size = 128 * 1024;
    let sector_size = 1024;

    let mut new_rom = firmware.data.to_vec();
    while new_rom.len() < rom_size {
        new_rom.push(0xFF);
    }

    let mut spi_bus = ec.spi(target, scratch)?;
    let mut spi = SpiRom::new(
        &mut spi_bus,
        StdTimeout::new(Duration::new(1, 0))
    );

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

unsafe fn flash(path: &str) -> Result<(), Error> {
    let target = SpiTarget::Main;
    let scratch = true;

    //TODO: remove unwraps
    let firmware_data = fs::read(path).unwrap();
    let firmware = Firmware::new(&firmware_data).unwrap();
    println!("file board: {:?}", str::from_utf8(firmware.board));
    println!("file version: {:?}", str::from_utf8(firmware.version));

    iopl();

    let mut ec = Ec::new(
        StdTimeout::new(Duration::new(1, 0)),
    )?;

    {
        let mut data = [0; 256];
        let size = ec.board(&mut data)?;

        let ec_board = &data[..size];
        println!("ec board: {:?}", str::from_utf8(ec_board));

        if ec_board != firmware.board {
            panic!("file board does not match ec board");
        }
    }

    {
        let mut data = [0; 256];
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
    iopl();

    let mut ec = Ec::new(
        StdTimeout::new(Duration::new(1, 0)),
    )?;

    {
        print!("board: ");
        let mut data = [0; 256];
        let size = ec.board(&mut data)?;
        for &b in data[..size].iter() {
            print!("{}", b as char);
        }
        println!();
    }

    {
        print!("version: ");
        let mut data = [0; 256];
        let size = ec.version(&mut data)?;
        for &b in data[..size].iter() {
            print!("{}", b as char);
        }
        println!();
    }
    
    Ok(())
}

unsafe fn print(message: &[u8]) -> Result<(), Error> {
    iopl();

    let mut ec = Ec::new(
        StdTimeout::new(Duration::new(1, 0)),
    )?;

    ec.print(message)?;

    Ok(())
}

unsafe fn fan_get(index: u8) -> Result<(), Error> {
    iopl();

    let mut ec = Ec::new(
        StdTimeout::new(Duration::new(1, 0)),
    )?;

    let duty = ec.fan_get(index)?;
    eprintln!("{}", duty);

    Ok(())
}

unsafe fn fan_set(index: u8, duty: u8) -> Result<(), Error> {
    iopl();

    let mut ec = Ec::new(
        StdTimeout::new(Duration::new(1, 0)),
    )?;

    ec.fan_set(index, duty)
}

fn usage() {
    eprintln!("  console");
    eprintln!("  flash [file]");
    eprintln!("  fan [index] <duty>");
    eprintln!("  info");
    eprintln!("  print [message]");
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
            "fan" => match args.next() {
                Some(index_str) => match index_str.parse::<u8>() {
                    Ok(index) => match args.next() {
                        Some(duty_str) => match duty_str.parse::<u8>() {
                            Ok(duty) => match unsafe { fan_set(index, duty) } {
                                Ok(()) => (),
                                Err(err) => {
                                    eprintln!("failed to set fan {} to {}: {:X?}", index, duty, err);
                                    process::exit(1);
                                },
                            },
                            Err(err) => {
                                eprintln!("failed to parse '{}': {:X?}", duty_str, err);
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
                    },
                    Err(err) => {
                        eprintln!("failed to parse '{}': {:X?}", index_str, err);
                        process::exit(1);
                    },
                },
                None => {
                    eprintln!("no index provided");
                    process::exit(1);
                },
            },
            "flash" => match args.next() {
                Some(path) => match unsafe { flash(&path) } {
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
