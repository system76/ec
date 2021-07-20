// SPDX-License-Identifier: GPL-3.0-only

#include <avr/eeprom.h>

#include <board/config.h>

uint8_t eeprom_read_byte_or(uint8_t * address, uint8_t fallback) {
    uint8_t value = eeprom_read_byte(address);
    if (value == 0xFF) {
        return fallback;
    } else {
        return value;
    }
}

uint16_t eeprom_read_word_or(uint16_t * address, uint16_t fallback) {
    uint16_t value = eeprom_read_word(address);
    if (value == 0xFFFF) {
        return fallback;
    } else {
        return value;
    }
}

uint32_t eeprom_read_dword_or(uint32_t * address, uint32_t fallback) {
    uint32_t value = eeprom_read_dword(address);
    if (value == 0xFFFFFFFF) {
        return fallback;
    } else {
        return value;
    }
}

struct Config config_new() {
    #undef CFG_16
    #define CFG_16(N, V) .N = V ## U,

    #undef CFG_32
    #define CFG_32(N, V) .N = V ## UL,

    struct Config config = {
        CFGS
    };

    return config;
}

void config_load(struct Config * config) {
    uint16_t addr = 0;

    #undef CFG_16
    #define CFG_16(N, V) \
        config->N = eeprom_read_word_or((uint16_t *)addr, V); \
        addr += 2;

    #undef CFG_32
    #define CFG_32(N, V) \
        config->N = eeprom_read_dword_or((uint32_t *)addr, V); \
        addr += 4;

    CFGS
}

void config_save(struct Config * config) {
    uint16_t addr = 0;

    #undef CFG_16
    #define CFG_16(N, V) \
        eeprom_update_word((uint16_t *)addr, config->N); \
        addr += 2;

    #undef CFG_32
    #define CFG_32(N, V) \
        eeprom_update_dword((uint32_t *)addr, config->N); \
        addr += 4;

    CFGS
}
