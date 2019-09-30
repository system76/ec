#include <board/pmc.h>

void pmc_init(void) {
    *(PMC_1.control) = 0x41;
    *(PMC_2.control) = 0x41;
}
