#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

#include <scratch/pmc.h>

volatile uint8_t __xdata __at(0x103B) ECINDAR0;
volatile uint8_t __xdata __at(0x103C) ECINDAR1;
volatile uint8_t __xdata __at(0x103D) ECINDAR2;
volatile uint8_t __xdata __at(0x103E) ECINDAR3;
volatile uint8_t __xdata __at(0x103F) ECINDDR;

volatile uint8_t __xdata __at(0x1F01) ETWCFG;
volatile uint8_t __xdata __at(0x1F07) EWDKEYR;

uint8_t acpi_read(uint8_t addr) {
    uint8_t data = 0;

    switch (addr) {
        case 4:
           data = ECINDAR0;
           break;
        case 5:
           data = ECINDAR1;
           break;
        case 6:
           data = ECINDAR2;
           break;
        case 7:
           data = ECINDAR3;
           break;
        case 8:
           data = ECINDDR;
           break;
    }

    return data;
}

void acpi_write(uint8_t addr, uint8_t data) {
    switch (addr) {
        case 4:
           ECINDAR0 = data;
           break;
        case 5:
           ECINDAR1 = data;
           break;
        case 6:
           ECINDAR2 = data;
           break;
        case 7:
           ECINDAR3 = data;
           break;
        case 8:
           ECINDDR = data;
           break;
    }
}

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
            state = PMC_STATE_DEFAULT;
            switch (data) {
            case 0x80:
                state = PMC_STATE_ACPI_READ;
                break;
            case 0x81:
                state = PMC_STATE_ACPI_WRITE;
                break;
            case 0xEC:
                printf_tiny("RESET\n");
                // Attempt to trigger watchdog reset
                ETWCFG |= (1 << 5);
                EWDKEYR = 0;
                // Clear processor caches
                __asm__("mov 0xf7, #1");
                __asm__("nop");
                __asm__("mov 0xf7, #1");
                __asm__("nop");
                __asm__("mov 0xf7, #1");
                __asm__("nop");
                __asm__("mov 0xf7, #1");
                __asm__("nop");
                // Exit scratch ROM by going through trampoline
                __asm__("ljmp 0x1000");
               break;
            }
        } else {
            switch (state) {
            case PMC_STATE_ACPI_READ:
                state = PMC_STATE_DEFAULT;
                uint8_t value = acpi_read(data);
                pmc_write(pmc, value);
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

// Main program while running in scratch ROM
void main(void) {
    printf_tiny("SCRATCH\n");
	for (;;) {
        pmc_event(&PMC_3);
    }
}
