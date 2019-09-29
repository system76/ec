#include "include/pmc.h"

#define PMC(NUM) { \
    .status = &PM ## NUM ## STS, \
    .data_out = &PM ## NUM ## DO, \
    .data_in = &PM ## NUM ## DI, \
    .control = &PM ## NUM ## CTL, \
}

__code struct Pmc PMC_1 = PMC(1);
__code struct Pmc PMC_2 = PMC(2);

void pmc_init(void) {
    *(PMC_1.control) = 0x41;
    *(PMC_2.control) = 0x41;
}

unsigned char pmc_status(struct Pmc * pmc) {
    return *(pmc->status);
}

unsigned char pmc_read(struct Pmc * pmc) {
    return *(pmc->data_in);
}

void pmc_write(struct Pmc * pmc, unsigned char data) {
    *(pmc->data_out) = data;
}
