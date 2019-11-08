#include <stdio.h>

#include <board/acpi.h>
#include <ec/pmc.h>

#define PMC(NUM) { \
    .status = &PM ## NUM ## STS, \
    .data_out = &PM ## NUM ## DO, \
    .data_in = &PM ## NUM ## DI, \
    .control = &PM ## NUM ## CTL, \
}

struct Pmc __code PMC_1 = PMC(1);
struct Pmc __code PMC_2 = PMC(2);

uint8_t pmc_status(struct Pmc * pmc) {
    return *(pmc->status);
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

#define PMC_TIMEOUT 1000

enum PmcState {
    PMC_STATE_DEFAULT,
    PMC_STATE_ACPI_READ,
    PMC_STATE_ACPI_WRITE,
    PMC_STATE_ACPI_WRITE_ADDR,
};

void pmc_event(struct Pmc * pmc) {
    static enum PmcState state = PMC_STATE_DEFAULT;
    static uint8_t state_data[2] = {0, 0};

    uint8_t sts = pmc_status(pmc);
    if (sts & PMC_STS_IBF) {
        uint8_t data = pmc_read(pmc);
        if (sts & PMC_STS_CMD) {
            printf("pmc cmd: %02X\n", data);

            state = PMC_STATE_DEFAULT;
            switch (data) {
            case 0x80:
                state = PMC_STATE_ACPI_READ;
                break;
            case 0x81:
                state = PMC_STATE_ACPI_WRITE;
                break;
            case 0x82:
                printf("  burst enable\n");
                // TODO: figure out what burst is
                pmc_write(pmc, 0x90, PMC_TIMEOUT);
                break;
            case 0x83:
                printf("  burst disable\n");
                // TODO: figure out what burst is
                break;
            case 0x84:
                printf("  SCI queue\n");
                // TODO: queue is always empty
                pmc_write(pmc, 0, PMC_TIMEOUT);
                break;
            }
        } else {
            printf("pmc data: %02X\n", data);

            switch (state) {
            case PMC_STATE_ACPI_READ:
                state = PMC_STATE_DEFAULT;
                uint8_t value = acpi_read(data);
                pmc_write(pmc, value, PMC_TIMEOUT);
                break;
            case PMC_STATE_ACPI_WRITE:
                state = PMC_STATE_ACPI_WRITE_ADDR;
                state_data[0] = data;
                break;
            case PMC_STATE_ACPI_WRITE_ADDR:
                state = PMC_STATE_DEFAULT;
                acpi_write(state_data[0], data);
                break;
            default:
                state = PMC_STATE_DEFAULT;
                break;
            }
        }
    }
}
