use ectool::{
    Ec,
    Error,
    Timeout,
};
use std::{
    io,
    process,
    time::{Duration, Instant},
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

unsafe fn tool() -> Result<(), Error> {
    let mut ec = Ec::new(
        true,
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

fn main() {
    extern {
        fn iopl(level: isize) -> isize;
    }

    if unsafe { iopl(3) < 0 } {
        eprintln!("failed to get I/O permission: {}", io::Error::last_os_error());
        process::exit(1);
    }

    match unsafe { tool() } {
        Ok(()) => (),
        Err(err) => {
            eprintln!("failed to run tool: {:X?}", err);
            process::exit(1);
        }
    }
}
