#include <board/smbus.h>

void smbus_init(void) {
    // 9.2 MHz * 4.7 us = 43.24
    SMB4P7USL = 43;
    // 9.2 MHz * 4.0 us = 36.8
    SMB4P0USL = 37;
    // 9.2 MHz * 300 ns = 2.76
    SMB300NS = 3;
    // 9.2 MHz * 250 ns = 2.3
    SMB250NS = 2;
    // 1.024 KHz * 25 ms = 25.6
    SMB25MS = 26;
    // 9.2 MHz * 45.3 us = 416.76 (0x01A1)
    SMB45P3USL = 0xA1;
    SMB45P3USH = 0x01;

    // Clock set to 50 Hz
    SCLKTSA = 1;

    // Enable host interface
    HOCTL2A = 1;
}
