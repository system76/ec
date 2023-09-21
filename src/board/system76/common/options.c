// SPDX-License-Identifier: GPL-3.0-only

#include <board/flash.h>
#include <board/options.h>
#include <common/debug.h>

uint8_t __xdata OPTIONS[NUM_OPTIONS];
uint8_t DEFAULT_OPTIONS[NUM_OPTIONS] = { 0 };

// Config is in the second to last sector of flash
const uint32_t OPTIONS_ADDR = 0x1F800;
// Signature is the size of the config
const uint16_t OPTIONS_SIGNATURE = sizeof(OPTIONS);

void options_init(void) {
    if (!options_load_config()) {
        options_load_default();
    }
}

void options_load_default(void) {
    for (uint8_t opt = 0; opt < NUM_OPTIONS; opt++) {
        OPTIONS[opt] = DEFAULT_OPTIONS[opt];
    }
}

bool options_erase_config(void) {
    // This will erase 1024 bytes
    flash_erase(OPTIONS_ADDR);

    // Verify signature is erased
    return flash_read_u16(OPTIONS_ADDR) == 0xFFFF;
}

bool options_load_config(void) {
    // Check signature
    if (flash_read_u16(OPTIONS_ADDR) != OPTIONS_SIGNATURE)
        return false;

    // Read the options if signature is valid
    flash_read(OPTIONS_ADDR + sizeof(OPTIONS_SIGNATURE), (uint8_t *)OPTIONS, sizeof(OPTIONS));
    return true;
}

bool options_save_config(void) {
    // Erase config region
    if (!options_erase_config())
        return false;

    // Write the options
    flash_write(OPTIONS_ADDR + sizeof(OPTIONS_SIGNATURE), (uint8_t *)OPTIONS, sizeof(OPTIONS));

    // Write the length of the options, as a signature
    flash_write_u16(OPTIONS_ADDR, OPTIONS_SIGNATURE);

    // Verify signature is valid
    return flash_read_u16(OPTIONS_ADDR) == OPTIONS_SIGNATURE;
}

// TODO error handling
uint8_t options_get(uint16_t index) {
    if (index < NUM_OPTIONS) {
        TRACE("OPTION %x READ %x\n", index, OPTIONS[index]);
        return OPTIONS[index];
    } else {
        return 0;
    }
}

bool options_set(uint16_t index, uint8_t value) {
    if (index < NUM_OPTIONS && OPTIONS[value] != value) {
        OPTIONS[index] = value;
        TRACE("OPTION %x WRITE %x\n", index, value);
        return options_save_config();
    } else {
        return false;
    }
}
