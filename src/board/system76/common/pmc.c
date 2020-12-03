// SPDX-License-Identifier: GPL-3.0-only

#include <arch/delay.h>
#include <board/acpi.h>
#include <board/gpio.h>
#include <board/pmc.h>
#include <common/debug.h>
#include <ec/espi.h>

void pmc_init(void) {
    *(PMC_1.control) = 0x41;
    *(PMC_2.control) = 0x41;
}

enum PmcState {
    PMC_STATE_DEFAULT,
    PMC_STATE_WRITE,
    PMC_STATE_ACPI_READ,
    PMC_STATE_ACPI_WRITE,
    PMC_STATE_ACPI_WRITE_ADDR,
};

static uint8_t pmc_sci_queue = 0;

static void pmc_sci_interrupt(void) {
#if EC_ESPI
    // Start SCI interrupt
    vw_set(&VW_SCI_N, VWS_LOW);

    // Delay T_HOLD (value assumed)
    delay_us(65);

    // Stop SCI interrupt
    vw_set(&VW_SCI_N, VWS_HIGH);

    // Delay T_HOLD (value assumed)
    delay_us(65);
#else // EC_ESPI
    // Start SCI interrupt
    gpio_set(&SCI_N, false);
    *(SCI_N.control) = GPIO_OUT;

    // Delay T_HOLD (value assumed)
    delay_us(65);

    // Stop SCI interrupt
    *(SCI_N.control) = GPIO_IN;
    gpio_set(&SCI_N, true);

    // Delay T_HOLD (value assumed)
    delay_us(65);
#endif // EC_ESPI
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

void pmc_swi(void) {
#if EC_ESPI
    // Use SCI interrupt
    pmc_sci_interrupt();
#else // EC_ESPI
    // Start SWI interrupt
    gpio_set(&SWI_N, false);

    // Delay T_HOLD (value assumed)
    delay_us(65);

    // Stop SWI interrupt
    gpio_set(&SWI_N, true);

    // Delay T_HOLD (value assumed)
    delay_us(65);
#endif // EC_ESPI
}

static enum PmcState state = PMC_STATE_DEFAULT;
static uint8_t state_data = 0;

static void pmc_on_input_command(struct Pmc * pmc, uint8_t data) {
    TRACE("pmc cmd: %02X\n", data);
    state = PMC_STATE_DEFAULT;
    switch (data) {
    case 0x80:
        state = PMC_STATE_ACPI_READ;
        // Send SCI for IBF=0
        pmc_sci_interrupt();
        break;
    case 0x81:
        state = PMC_STATE_ACPI_WRITE;
        // Send SCI for IBF=0
        pmc_sci_interrupt();
        break;
    case 0x82:
        TRACE("  burst enable\n");
        // Set burst bit
        pmc_set_status(pmc, pmc_status(pmc) | (1 << 4));
        // Send acknowledgement byte
        state = PMC_STATE_WRITE;
        state_data = 0x90;
        break;
    case 0x83:
        TRACE("  burst disable\n");
        // Clear burst bit
        pmc_set_status(pmc, pmc_status(pmc) & ~(1 << 4));
        // Send SCI for IBF=0
        pmc_sci_interrupt();
        break;
    case 0x84:
        TRACE("  SCI queue\n");
        // Clear SCI pending bit
        pmc_set_status(pmc, pmc_status(pmc) & ~(1 << 5));
        // Send SCI queue
        state = PMC_STATE_WRITE;
        state_data = pmc_sci_queue;
        // Clear SCI queue
        pmc_sci_queue = 0;
        break;
    }
}

static void pmc_on_input_data(uint8_t data) {
    TRACE("pmc data: %02X\n", data);
    switch (state) {
    case PMC_STATE_ACPI_READ:
        // Send byte from ACPI space
        state = PMC_STATE_WRITE;
        state_data = acpi_read(data);
        break;
    case PMC_STATE_ACPI_WRITE:
        state = PMC_STATE_ACPI_WRITE_ADDR;
        state_data = data;
        // Send SCI for IBF=0
        pmc_sci_interrupt();
        break;
    case PMC_STATE_ACPI_WRITE_ADDR:
        state = PMC_STATE_DEFAULT;
        acpi_write(state_data, data);
        // Send SCI for IBF=0
        pmc_sci_interrupt();
        break;
    default:
        state = PMC_STATE_DEFAULT;
        break;
    }
}

static void pmc_on_output_empty(struct Pmc * pmc) {
    switch (state) {
        case PMC_STATE_WRITE:
            TRACE("pmc write: %02X\n", state_data);
            state = PMC_STATE_DEFAULT;
            pmc_write(pmc, state_data);
            // Send SCI for OBF=1
            pmc_sci_interrupt();
            break;
    }
}

void pmc_event(struct Pmc * pmc) {
    uint8_t sts;

    // Read command/data if available
    sts = pmc_status(pmc);
    if (sts & PMC_STS_IBF) {
        uint8_t data = pmc_read(pmc);
        if (sts & PMC_STS_CMD) {
            pmc_on_input_command(pmc, data);
        } else {
            pmc_on_input_data(data);
        }
    }

    // Write data if possible
    sts = pmc_status(pmc);
    if (!(sts & PMC_STS_OBF)) {
        pmc_on_output_empty(pmc);
    }
}
