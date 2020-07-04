/*
 * Copyright (C) 2020 Evan Lojewski
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <board/flash.h>
#include <common/config.h>
#include <common/debug.h>
#include <stddef.h>
#include <string.h>

// Prevent failures to compile on AVR
#ifndef __SDCC
    #define __code
#endif

config_t *g_config_entries = NULL;

#define CONFIG_ADDRESS  (0x10000L)
#define CONFIG_SIZE   (1 * 1024)  /* 1K block - code currently assumes this matches the erase size */
#define CONFIG_PERSISTANT_MAGIC     (0xCF1C0F11)
#define CONFIG_HEADER_SIZE          (sizeof(uint32_t) + 4) /* Magic number plus four reserved bytes */

#define CONFIG_ENTRIES_START        (CONFIG_ADDRESS + CONFIG_HEADER_SIZE)

uint32_t __code __at(CONFIG_ADDRESS) config_magic;
uint8_t __code __at(CONFIG_ENTRIES_START) config_persistant[CONFIG_SIZE - CONFIG_HEADER_SIZE];

#define CONFIG_FLASH_ENTRY_EMPTY            (0xFFFFFFFF)
#define CONFIG_FLASH_ENTRY_CURRENT          (0xFF)
#define CONFIG_FLASH_ENTRY_OBSOLETE         (0x00)

// config_persistant format
// (4) bytes: Configuration ID, 0xFFFFFFFF when an entry is not set.
// (1) bytes: Entry current (valid) or obsolete (invalid)
// (1) bytes: Entry type (0xFF: 32bit value follows)
// (4) bytes: Configuration Value
#define CONFIG_FLASH_ENTRY_ID_OFFSET        (0)
#define CONFIG_FLASH_ENTRY_CURRENT_OFFSET   (4)
#define CONFIG_FLASH_ENTRY_TYPE_OFFSET      (5) /* Unused - RFU */
#define CONFIG_FLASH_ENTRY_VALUE_OFFSET     (6)
#define CONFIG_FLASH_ENTRY_SIZE             (10)

uint8_t config_validate_magic(void) {
    uint32_t magic = flash_read_u32(CONFIG_ADDRESS);

    return (CONFIG_PERSISTANT_MAGIC == magic);
}

uint32_t config_find_free_entry() {
    if (!config_validate_magic()) {
        // Write magic.
        flash_write_u32(CONFIG_ADDRESS, CONFIG_PERSISTANT_MAGIC);

        // Check that that magic wrote properly. If not, we'll need to erase.
        if (!config_validate_magic()) {
            return 0;
        }
    }

    uint32_t addr = CONFIG_ENTRIES_START;
    uint32_t limit = addr + sizeof(config_persistant);
    while(addr < limit) {
        // configuration present.
        uint32_t id;
        id = flash_read_u32(addr + CONFIG_FLASH_ENTRY_ID_OFFSET);

        if(CONFIG_FLASH_ENTRY_EMPTY == id) {
            // Free entry found.
            return addr;
        }

        // Go to the next entry.
        addr += CONFIG_FLASH_ENTRY_SIZE;
    }

    return 0;
}

void config_save_entry_id(uint32_t addr, uint32_t id) {
    flash_write_u32(addr + CONFIG_FLASH_ENTRY_ID_OFFSET, id);
}

void config_save_entry_value(uint32_t addr, int32_t value) {
    flash_write_u32(addr + CONFIG_FLASH_ENTRY_VALUE_OFFSET, value);
}

bool config_save_entry(config_t *entry) {
    uint32_t id = (uint32_t)entry->config_id[3] << 24 | (uint32_t)entry->config_id[2] << 16 | (uint32_t)entry->config_id[1] << 8 | entry->config_id[0];
    int32_t value = entry->value.value;
    uint32_t addr = config_find_free_entry();

    if (addr && ((addr + CONFIG_FLASH_ENTRY_SIZE - CONFIG_ADDRESS) < sizeof(config_persistant))) {
        DEBUG("Saving config: %s", entry->config_short);
        DEBUG(" = %ld\n", value);

        config_save_entry_id(addr, id);

        config_save_entry_value(addr, value);

        return true;
    } else {
        return false;
    }
}

bool config_save_entries() __reentrant {
    bool status = true;

    flash_erase(CONFIG_ADDRESS);

    config_t *entry = g_config_entries;

    while (entry) {
        if (!config_save_entry(entry)) {
            status = false;
        }
        entry = entry->next;
    }

    return status;
}

bool config_flash_valid(uint32_t addr) {
    return (CONFIG_FLASH_ENTRY_CURRENT == flash_read_u8(addr + CONFIG_FLASH_ENTRY_CURRENT_OFFSET));
}

uint32_t config_flash_id(uint32_t addr) {
    return flash_read_u32(addr + CONFIG_FLASH_ENTRY_ID_OFFSET);
}

int32_t config_flash_value(uint32_t addr) {
    return flash_read_u32(addr + CONFIG_FLASH_ENTRY_VALUE_OFFSET);
}

uint32_t condif_flash_next(uint32_t addr) {
    return addr + CONFIG_FLASH_ENTRY_SIZE;
}

uint32_t config_find_entry(const unsigned char config_id[4]) {
    uint32_t int_id = (uint32_t)config_id[3] << 24 | (uint32_t)config_id[2] << 16 | (uint32_t)config_id[1] << 8 | config_id[0];

    if(config_validate_magic()) {
        uint32_t addr = CONFIG_ENTRIES_START;
        uint32_t limit = addr + sizeof(config_persistant);

        while(addr < limit) {
            uint32_t id;
            // configuration present.
            id = config_flash_id(addr);

            if(id == CONFIG_FLASH_ENTRY_EMPTY) {
                // no more entries exist.
                break;
            }

            if(config_flash_valid(addr)) {
                if(int_id == id) {
                    return addr;
                }
            }

            // Go to the next entry.
            addr = condif_flash_next(addr);
        }
    }

    // Not found.
    return 0;
}

void config_invalidate_entry(uint32_t addr) {
    if (addr) {
        flash_write_u8(addr + CONFIG_FLASH_ENTRY_CURRENT_OFFSET, CONFIG_FLASH_ENTRY_OBSOLETE);
    }
}

bool config_register(config_t *entry) __reentrant {
    // Validate entry parameters.
    if (!entry) {
        return false;
    }

    // Short name and desc must be set.
    if (!entry->config_short || !entry->config_desc) {
        return false;
    }

    // Config validated, register it
    entry->next = g_config_entries;
    g_config_entries = entry;

    // Read the stored value from flash, if present.
    uint32_t flash_addr = config_find_entry(entry->config_id);
    if (flash_addr) {
        int32_t saved_value = config_flash_value(flash_addr);

        int64_t min = entry->value.min_value;
        int64_t max = entry->value.max_value;

        if (min <= saved_value &&
            max >= saved_value) {
            entry->value.value = saved_value;

            /* Notify any listeners. */
            if (entry->set_callback) {
                entry->set_callback(entry);
            }
        }
    }


    return true;
}

config_t *config_get_config(const char *config_short) {
    config_t *current = g_config_entries;

    while (current && 0 != strcmp(current->config_short, config_short)) {
        current = current->next;
    }

    return current;
}

config_t *config_next(config_t *current) {
    if (current) {
        current = current->next;
    }

    return current;
}

config_t *config_index(int32_t index) {
    config_t *current = g_config_entries;

    while (current && index) {
        current = current->next;
        index = index - 1;
    }

    return current;
}

int32_t config_get_value(config_t *config) __reentrant {
    if (config) {
        return config->value.value;
    }
    else {
        // Invalid
        return 0;
    }

}

bool config_set_value(config_t *config, int32_t value) __reentrant {
    bool valid = false;

    if (config) {
        bool saved = true;
        int64_t min = config->value.min_value;
        int64_t max = config->value.max_value;

        if (min <= value &&
            max >= value) {
            config->value.value = value;
            valid = true;

            /* Save the new value to flash, if it's changed */
            uint32_t flash_addr = config_find_entry(config->config_id);

            if (flash_addr) {
                int32_t oldval = config_flash_value(flash_addr);

                if(oldval != value) {
                    config_invalidate_entry(flash_addr);
                    saved = config_save_entry(config);
                }
            } else {
                // No entry, create one.
                saved = config_save_entry(config);
            }

            /* Notify any listeners. */
            if (config->set_callback) {
                config->set_callback(config);
            }
        }

        if (!saved) {
            // Not enough space exists. clear out flash and re-write.
            flash_erase(CONFIG_ADDRESS);

            if (!config_save_entries()) {
                valid = false;
            }

        }
    }

    return valid;
}
