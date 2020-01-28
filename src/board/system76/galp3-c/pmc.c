#include <arch/delay.h>
#include <board/acpi.h>
#include <board/gpio.h>
#include <board/pmc.h>
#include <board/scratch.h>
#include <common/debug.h>

void pmc_init(void) {
    *(PMC_1.control) = 0x41;
    *(PMC_2.control) = 0x41;
}

#define PMC_TIMEOUT 10000

enum PmcState {
    PMC_STATE_DEFAULT,
    PMC_STATE_ACPI_READ,
    PMC_STATE_ACPI_WRITE,
    PMC_STATE_ACPI_WRITE_ADDR,
};

static uint8_t pmc_sci_queue = 0;

void pmc_sci_interrupt(void) {
    // Start SCI interrupt
    gpio_set(&SCI_N, false);
    *(SCI_N.control) = GPIO_OUT;

    // Delay T_HOLD (value assumed)
    delay_us(1);

    // Stop SCI interrupt
    *(SCI_N.control) = GPIO_IN;
    gpio_set(&SCI_N, true);
}

bool pmc_sci(struct Pmc * pmc, uint8_t sci) {
    // Set SCI queue if possible
    if (pmc_sci_queue == 0) {
        pmc_sci_queue = sci;

        // Set SCI pending bit
        pmc_set_status(pmc, pmc_status(pmc) | (1 << 5));

        // Send SCI
        pmc_sci_interrupt();

        return true;
    } else {
        return false;
    }
}

void pmc_event(struct Pmc * pmc) {
    static enum PmcState state = PMC_STATE_DEFAULT;
    static uint8_t state_data = 0;

    uint8_t burst_timeout;
    for (burst_timeout = 1; burst_timeout > 0; burst_timeout--) {
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
                    // Run pmc_event in a tight loop for more iterations
                    burst_timeout = 100;
                    // Set burst bit
                    pmc_set_status(pmc, sts | (1 << 4));
                    // Send acknowledgement byte
                    pmc_write(pmc, 0x90, PMC_TIMEOUT);
                    break;
                case 0x83:
                    DEBUG("  burst disable\n");
                    // Exit pmc_event tight loop
                    burst_timeout = 0;
                    // Clear burst bit
                    pmc_set_status(pmc, sts & ~(1 << 4));
                    break;
                case 0x84:
                    DEBUG("  SCI queue\n");
                    // Clear SCI pending bit
                    pmc_set_status(pmc, sts & ~(1 << 5));
                    // Send SCI queue
                    pmc_write(pmc, pmc_sci_queue, PMC_TIMEOUT);
                    // Clear SCI queue
                    pmc_sci_queue = 0;
                    break;

                case 0xEC:
                    DEBUG("  scratch rom\n");
                    pmc_write(pmc, 0x76, PMC_TIMEOUT);
                    scratch_trampoline();
                    break;
                }
            } else {
                DEBUG("pmc data: %02X\n", data);

                switch (state) {
                case PMC_STATE_ACPI_READ:
                    state = PMC_STATE_DEFAULT;
                    state_data = acpi_read(data);
                    pmc_write(pmc, state_data, PMC_TIMEOUT);
                    break;
                case PMC_STATE_ACPI_WRITE:
                    state = PMC_STATE_ACPI_WRITE_ADDR;
                    state_data = data;
                    break;
                case PMC_STATE_ACPI_WRITE_ADDR:
                    state = PMC_STATE_DEFAULT;
                    acpi_write(state_data, data);
                    break;
                default:
                    state = PMC_STATE_DEFAULT;
                    break;
                }
            }
        }
    }
}
