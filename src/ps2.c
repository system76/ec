#include "include/ps2.h"

void ps2_init(void) {
    PSCTL1 = 0x11;
    PSCTL2 = 0x41;
    PSCTL3 = 0x41;
    PSINT1 = 0x04;
    PSINT2 = 0x04;
    PSINT3 = 0x04;
}
