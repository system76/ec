#[derive(Debug)]
pub enum Error {
    Protocol(u8),
    Signature((u8, u8)),
    SuperIoId(u16),
    Timeout,
    WouldBlock,
}

