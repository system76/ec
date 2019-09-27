#include "include/kbc.h"

void kbc_init(void) {
    KBIRQR = 0;
    KBHICR = 0x48;
}
