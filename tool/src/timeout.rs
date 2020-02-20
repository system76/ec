pub trait Timeout {
    fn reset(&mut self);
    fn running(&self) -> bool;
}
