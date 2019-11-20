#include <board/acpi.h>
#include <board/pmc.h>
#include <board/scratch.h>
#include <common/debug.h>

void pmc_init(void) {
    *(PMC_1.control) = 0x41;
    *(PMC_2.control) = 0x41;
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
            DEBUG("pmc cmd: %02X\n", data);

            state = PMC_STATE_DEFAULT;
            switch (data) {
            case 0x80:
                state = PMC_STATE_ACPI_READ;
                break;
            case 0x81:
                state = PMC_STATE_ACPI_WRITE;
                break;
            case 0x82:
                DEBUG("  burst enable\n");
                // TODO: figure out what burst is
                pmc_write(pmc, 0x90, PMC_TIMEOUT);
                break;
            case 0x83:
                DEBUG("  burst disable\n");
                // TODO: figure out what burst is
                break;
            case 0x84:
                DEBUG("  SCI queue\n");
                // TODO: queue is always empty
                pmc_write(pmc, 0, PMC_TIMEOUT);
                break;

            case 0xDC:
                DEBUG("  scratch rom\n");
                pmc_write(pmc, 0x33, PMC_TIMEOUT);
                scratch_trampoline();
                break;
            }
        } else {
            DEBUG("pmc data: %02X\n", data);

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
