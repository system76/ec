#[macro_export]
macro_rules! timeout {
    ($t:expr, $f:expr) => {{
        let mut result = Err($crate::Error::Timeout);
        while $t.running() {
            match $f {
                Ok(ok) => {
                    result = Ok(ok);
                    break;
                },
                Err(err) => match err {
                    $crate::Error::WouldBlock => (),
                    _ => {
                        result = Err(err);
                        break;
                    }
                },
            }
        }
        result
    }};
}

pub trait Timeout {
    fn reset(&mut self);
    fn running(&self) -> bool;
}
