#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>

#include "include/delay.h"
#include "include/gpio.h"
#include "include/gctrl.h"
#include "include/kbc.h"
#include "include/pin.h"
#include "include/pmc.h"
#include "include/ps2.h"
#include "include/kbscan.h"

void external_0(void) __interrupt(0) {
    printf("external_0\n");
}

void timer_0(void) __interrupt(1) {
    printf("timer_0\n");
}

void external_1(void) __interrupt(2) {
    printf("external_1\n");
}

void timer_1(void) __interrupt(3) {
    printf("timer_1\n");
}

void serial(void) __interrupt(4) {
    printf("serial\n");
}

void timer_2(void) __interrupt(5) {
    printf("timer_2\n");
}

void init(void) {
    gpio_init();
    gctrl_init();
    kbc_init();
    pmc_init();
    kbscan_init();

    //TODO: INTC, PECI, PWM, SMBUS

    // PECI information can be found here: https://www.intel.com/content/dam/www/public/us/en/documents/design-guides/core-i7-lga-2011-guide.pdf
}

struct Pin PWR_SW = PIN(D, 0);

void power_switch(void) {
    static bool last = false;

    // Check if the power switch goes low
    bool new = pin_get(&PWR_SW);
    if (!new && last) {
        printf("Power Switch\n");
    }
    last = new;
}

void kbc_event(struct Kbc * kbc) {
    uint8_t sts = kbc_status(kbc);
    if (sts & KBC_STS_IBF) {
        uint8_t data = kbc_read(kbc);
        if (sts & KBC_STS_CMD) {
            printf("KBC command: %X\n", data);
        } else {
            printf("KBC data: %X\n", data);
        }
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
            printf("PMC command: %02X\n", data);

            switch (data) {
            case 0x80:
                state = PMC_STATE_ACPI_READ;
                printf("  ACPI read\n");
                break;
            case 0x81:
                state = PMC_STATE_ACPI_WRITE;
                printf("  ACPI write\n");
                break;
            default:
                state = PMC_STATE_DEFAULT;
                printf("  Unknown command\n");
                break;
            }
        } else {
            printf("PMC data: %02X\n", data);

            switch (state) {
            case PMC_STATE_ACPI_READ:
                state = PMC_STATE_DEFAULT;
                //TODO - read value
                uint8_t value = 0;
                pmc_write(pmc, value);
                printf("  ACPI read %02X = %02X\n", data, value);
                break;
            case PMC_STATE_ACPI_WRITE:
                state = PMC_STATE_ACPI_WRITE_ADDR;
                state_data[0] = data;
                printf("  ACPI write %02X\n", data);
                break;
            case PMC_STATE_ACPI_WRITE_ADDR:
                state = PMC_STATE_DEFAULT;
                //TODO - write value
                printf("  ACPI write %02X = %02X\n", state_data[0], data);
                break;
            default:
                state = PMC_STATE_DEFAULT;
                printf("  Unknown data\n");
                break;
            }
        }
    }
}

struct Pin LED_BAT_CHG = PIN(A, 5);
struct Pin LED_BAT_FULL = PIN(A, 6);
struct Pin LED_PWR = PIN(A, 7);
struct Pin LED_ACIN = PIN(C, 7);
struct Pin LED_AIRPLANE_N = PIN(G, 6);

__code const char * MODEL = "galp3-c";

void main(void) {
    init();

    // Set the battery full LED (to know our firmware is loaded)
    pin_set(&LED_BAT_FULL, true);
    printf("Hello from System76 EC for %s!\n", MODEL);

    gpio_debug();

    for(;;) {
        power_switch();
        kbc_event(&KBC);
        pmc_event(&PMC_1);
    }
}
