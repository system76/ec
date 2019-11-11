#include <board/smbus.h>

void smbus_init(void) {
    SMB4P7USL = 0x2F;
    SMB4P0USL = 0x5A;
    SMB300NS = 0x03;
    SMB250NS = 0x30;
    SMB25MS = 0x19;
    SMB45P3USL = 0x5C;
    SMB45P3USH = 0x01;

    // Clock to 50 KHz
    SCLKTSA = 1;

    // Enable host interface
    HOCTL2A = 1 << 0;
}
