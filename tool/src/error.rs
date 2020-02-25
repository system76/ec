#[derive(Debug)]
pub enum Error {
    Parameter,
    Protocol(u8),
    Signature((u8, u8)),
    SuperIoId(u16),
    Timeout,
    Verify,
    WouldBlock,
}

