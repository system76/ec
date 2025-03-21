// SPDX-License-Identifier: GPL-3.0-only

#include <board/flash.h>
#include <board/options.h>
#include <common/debug.h>
#include <common/macro.h>

uint8_t __xdata OPTIONS[NUM_OPTIONS];

// clang-format off
uint8_t DEFAULT_OPTIONS[NUM_OPTIONS] = {
    [OPT_POWER_ON_AC] = 0,
    [OPT_KBLED_LEVEL_I] = 0,
    [OPT_KBLED_COLOR_I] = 0,
    [OPT_BAT_THRESHOLD_START] = 95,
    [OPT_BAT_THRESHOLD_STOP] = 98,
    [OPT_ALLOW_BAT_BOOST] = 0,
    [OPT_ALWAYS_ON_USB] = 0,
    [OPT_GPU_MUX_CTRL] = 0,
};
// clang-format on

// Config is in the second to last sector of flash
const uint32_t OPTIONS_ADDR = 0x1F800;
// Signature is the size of the config
const uint16_t OPTIONS_SIGNATURE = sizeof(OPTIONS);

void options_reset() {
    for (uint8_t opt = 0; opt < NUM_OPTIONS; opt++) {
        OPTIONS[opt] = DEFAULT_OPTIONS[opt];
    }
}

static bool options_load_config() {
    // Check signature
    if (flash_read_u16(OPTIONS_ADDR) != OPTIONS_SIGNATURE)
        return false;

    // Read the options if signature is valid
    flash_read(OPTIONS_ADDR + sizeof(OPTIONS_SIGNATURE), (uint8_t *)OPTIONS, sizeof(OPTIONS));
    return true;
}

static bool options_erase_config(void) {
    // This will erase 1024 bytes
    flash_erase(OPTIONS_ADDR);

    // Verify signature is erased
    return flash_read_u16(OPTIONS_ADDR) == 0xFFFF;
}

void options_init(void) {
    if (!options_load_config()) {
        options_reset();
    }
}

static bool options_changed() {
    uint8_t current[NUM_OPTIONS];
    // Check if anything changed
    if (flash_read_u16(OPTIONS_ADDR) != OPTIONS_SIGNATURE) {
        return (true);
    } else {
        // Read the options if signature is valid
        flash_read(OPTIONS_ADDR + sizeof(OPTIONS_SIGNATURE), current, sizeof(current));
        for (uint8_t i = 0; i < NUM_OPTIONS; ++i) {
            if (current[i] != OPTIONS[i]) {
                return (true);
            }
        }
    }
    return false;
}

static bool options_save_config(void) {
    // Bail if no settings changed to save flash write cycles
    if (!options_changed())
        return true;

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
    if (index < NUM_OPTIONS) {
        TRACE("OPTION %x WRITE %x\n", index, value);
        OPTIONS[index] = value;
        return options_save_config();
    } else {
        return false;
    }
}
