#include "include/pmc.h"

void pmc_init(void) {
    PM1CTL = 0x41;
    PM2CTL = 0x41;
}
