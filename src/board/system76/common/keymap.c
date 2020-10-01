#include <board/flash.h>
#include <board/keymap.h>

uint16_t __xdata DYNAMIC_KEYMAP[KM_LAY][KM_OUT][KM_IN];

// Config is in the last sector of flash
const uint32_t CONFIG_ADDR = 0x1FC00;
// Signature is the size of the keymap
const uint16_t CONFIG_SIGNATURE = sizeof(DYNAMIC_KEYMAP);

void keymap_init(void) {
    if (!keymap_load_config()) {
        keymap_load_default();
    }
}

void keymap_load_default(void) {
    for (int layer = 0; layer < KM_LAY; layer++) {
        for (int output = 0; output < KM_OUT; output++) {
            for (int input = 0; input < KM_IN; input++) {
                DYNAMIC_KEYMAP[layer][output][input] = KEYMAP[layer][output][input];
            }
        }
    }
}

bool keymap_erase_config(void) {
    // This will erase 1024 bytes
    flash_erase(CONFIG_ADDR);

    // Verify signature is erased
    return flash_read_u16(CONFIG_ADDR) == 0xFFFF;
}

bool keymap_load_config(void) {
    // Check signature
    if (flash_read_u16(CONFIG_ADDR) != CONFIG_SIGNATURE) return false;

    // Read the keymap if signature is valid
    flash_read(CONFIG_ADDR + sizeof(CONFIG_SIGNATURE), (uint8_t *)DYNAMIC_KEYMAP, sizeof(DYNAMIC_KEYMAP));
    return true;
}

bool keymap_save_config(void) {
    // Erase config region
    if (!keymap_erase_config()) return false;

    // Write the keymap
    flash_write(CONFIG_ADDR + sizeof(CONFIG_SIGNATURE), (uint8_t *)DYNAMIC_KEYMAP, sizeof(DYNAMIC_KEYMAP));

    // Write the length of the keymap, as a signature
    flash_write_u16(CONFIG_ADDR, CONFIG_SIGNATURE);

    // Verify signature is valid
    return flash_read_u16(CONFIG_ADDR) == CONFIG_SIGNATURE;
}

bool keymap_get(int layer, int output, int input, uint16_t * value) {
    if (layer < KM_LAY && output < KM_OUT && input < KM_IN) {
        *value = DYNAMIC_KEYMAP[layer][output][input];
        return true;
    } else {
        return false;
    }
}

bool keymap_set(int layer, int output, int input, uint16_t value) {
    if (layer < KM_LAY && output < KM_OUT && input < KM_IN) {
        DYNAMIC_KEYMAP[layer][output][input] = value;
        return true;
    } else {
        return false;
    }
}
