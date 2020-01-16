#include <ec/pmc.h>

#define PMC(NUM) { \
    .status = &PM ## NUM ## STS, \
    .data_out = &PM ## NUM ## DO, \
    .data_in = &PM ## NUM ## DI, \
    .control = &PM ## NUM ## CTL, \
}

struct Pmc __code PMC_1 = PMC(1);
struct Pmc __code PMC_2 = PMC(2);
struct Pmc __code PMC_3 = PMC(3);
struct Pmc __code PMC_4 = PMC(4);
struct Pmc __code PMC_5 = PMC(5);

uint8_t pmc_status(struct Pmc * pmc) {
    return *(pmc->status);
}

void pmc_set_status(struct Pmc * pmc, uint8_t status) {
    *(pmc->status) = status;
}

uint8_t pmc_read(struct Pmc * pmc) {
    return *(pmc->data_in);
}

static bool pmc_wait(struct Pmc * pmc, int timeout) {
    while (pmc_status(pmc) & PMC_STS_OBF) {
        if (timeout == 0) return false;
        timeout -= 1;
    }
    return true;
}

bool pmc_write(struct Pmc * pmc, uint8_t data, int timeout) {
    if (!pmc_wait(pmc, timeout)) return false;
    *(pmc->data_out) = data;
    return true;
}
