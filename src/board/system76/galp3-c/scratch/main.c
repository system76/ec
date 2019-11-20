#include <stdbool.h>
#include <stdint.h>

#include <scratch/pmc.h>

volatile uint8_t __xdata __at(0x103B) ECINDAR0;
volatile uint8_t __xdata __at(0x103C) ECINDAR1;
volatile uint8_t __xdata __at(0x103D) ECINDAR2;
volatile uint8_t __xdata __at(0x103E) ECINDAR3;
volatile uint8_t __xdata __at(0x103F) ECINDDR;

#define FLASH_SPI 0x00000000
#define FLASH_EMBEDDED 0x40000000

static int flash_transaction(uint32_t offset, uint8_t * data, int length, bool read) {
    int i;
    for (i = 0; i < length; i++, offset++) {
        ECINDAR3 = (uint8_t)(offset >> 24);
        ECINDAR2 = (uint8_t)(offset >> 16);
        ECINDAR1 = (uint8_t)(offset >> 8);
        ECINDAR0 = (uint8_t)(offset);
        if (read) {
            data[i] = ECINDDR;
        } else {
            ECINDDR = data[i];
        }
    }
    return i;
}

enum PmcState {
    PMC_STATE_DEFAULT,
    PMC_STATE_WRITE,
};

static uint8_t __xdata __at(0x1F07) EWDKEYR;

static void pmc_event(struct Pmc * pmc) {
    static enum PmcState state = PMC_STATE_DEFAULT;

    uint8_t sts = pmc_status(pmc);
    if (sts & PMC_STS_IBF) {
        uint8_t data = pmc_read(pmc);
        if (sts & PMC_STS_CMD) {
            switch (state) {
                case PMC_STATE_DEFAULT:
                    switch (data) {
                        case 0x01:
                            // Enable follow
                            ECINDAR3 = 0x0F;
                            break;
                        case 0x02:
                            // Generate high CE#
                            data = 0;
                            flash_transaction(0x7FFFFE00, &data, 1, false);
                            // Fall through
                        case 0x03:
                            state = PMC_STATE_WRITE;
                            break;
                        case 0x04:
                            // Read data
                            flash_transaction(0x7FFFFD00, &data, 1, true);
                            pmc_write(pmc, data);
                            break;
                        case 0x05:
                            // Disable follow
                            ECINDAR3 = 0x00;
                            break;
                        case 0xFC:
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
                    break;
                case PMC_STATE_WRITE:
                    // Write command or data
                    flash_transaction(0x7FFFFD00, &data, 1, false);
                    state = PMC_STATE_DEFAULT;
                    break;
            }
        }
    }
}

// Main program while running in scratch ROM
void main(void) {
	for (;;) {
        pmc_event(&PMC_1);
    }
}
