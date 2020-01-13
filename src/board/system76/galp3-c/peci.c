#include <stdbool.h>

#include <board/peci.h>
#include <common/debug.h>
#include <ec/pwm.h>

// Tjunction = 100C for i7-8565U (and probably the same for all WHL-U)
#define T_JUNCTION 100

int16_t peci_offset = 0;
int16_t peci_temp = 0;
uint8_t peci_duty = 0;
uint8_t peci_tcontrol = 0;
uint8_t peci_tjmax = T_JUNCTION;
static bool peci_config_loaded = false;

#define PECI_TEMP(X) (((int16_t)(X)) << 6)

// Read tjmax using index 16 of RdPkgConfig
static void peci_config(void) {
    // Wait for completion
    while (HOSTAR & 1) {}
    // Clear status
    HOSTAR  = HOSTAR;

    // Enable PECI, clearing data fifo's
    HOCTLR = (1 << 5) | (1 << 3);
    // Set address to default
    HOTRADDR = 0x30;
    // Set write length
    HOWRLR = 5;
    // Set read length
    HORDLR = 5;
    // Set command
    HOCMDR = 0xA1;
    // Set Host ID ?
    HOWRDR = 0x00;
    // Set index
    HOWRDR = 16;
    // Set parameter
    HOWRDR = 0;
    HOWRDR = 0;
    // Start transaction
    HOCTLR |= 1;

    // Wait for completion
    while (HOSTAR & 1) {}

    if (HOSTAR & (1 << 1)) {
        // Use result if finished successfully

        //TODO: check completion code
        uint8_t data = HOWRDR;

        // Throw away reserved byte
        data = HOWRDR;
        // Tead tcontrol for now
        peci_tcontrol = HOWRDR;
        // Read tjmax
        peci_tjmax = HOWRDR;
        // Throw away reserved byte
        data = HOWRDR;

        peci_config_loaded = true;
    }
}

// PECI information can be found here: https://www.intel.com/content/dam/www/public/us/en/documents/design-guides/core-i7-lga-2011-guide.pdf
void peci_event(void) {
    // Wait for completion
    while (HOSTAR & 1) {}
    // Clear status
    HOSTAR = HOSTAR;

    // Enable PECI, clearing data fifo's
    HOCTLR = (1 << 5) | (1 << 3);
    // Set address to default
    HOTRADDR = 0x30;
    // Set write length
    HOWRLR = 1;
    // Set read length
    HORDLR = 2;
    // Set command
    HOCMDR = 1;
    // Start transaction
    HOCTLR |= 1;

    // Wait for completion
    while (HOSTAR & 1) {}

    if (HOSTAR & (1 << 1)) {
        // Use result if finished successfully
        uint8_t low = HORDDR;
        uint8_t high = HORDDR;
        peci_offset = ((int16_t)high << 8) | (int16_t)low;

        // TODO: Update max value if possible
        // if (!peci_config_loaded) {
        //     peci_config();
        // }

        // TODO: tjmax
        peci_temp = PECI_TEMP(T_JUNCTION) + peci_offset;

        // Set fan based on temp, adapted from
        // https://github.com/pop-os/system76-power/blob/master/src/fan.rs#L218
        if (peci_temp >= PECI_TEMP(90)) {
            // 90C = 100%
            peci_duty = 255;
        } else if (peci_temp >= PECI_TEMP(80)) {
            // 80C = 50%
            peci_duty = 128;
        } else if (peci_temp >= PECI_TEMP(75)) {
            // 75C = 45%
            peci_duty = 115;
        } else if (peci_temp >= PECI_TEMP(65)) {
            // 65C = 40%
            peci_duty = 102;
        } else if (peci_temp >= PECI_TEMP(55)) {
            // 55C = 35%
            peci_duty = 90;
        } else if (peci_temp >= PECI_TEMP(45)) {
            // 45C = 30%
            peci_duty = 77;
        } else {
            // < 45C = 0%
            peci_duty = 0;
        }
    } else {
        // Default to 50% if there is an error
        peci_offset = 0;
        peci_temp = 0;
        peci_duty = 128;
    }

    if (peci_duty != DCR2) {
        DCR2 = peci_duty;
        DEBUG("PECI offset=%d, temp=%d = %d\n", peci_offset, peci_temp, peci_duty);
    }
}
