/*
 * Copyright (C) 2020 Evan Lojewski
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <stdint.h>
#include <stddef.h>
#include <board/flash.h>
#include <ec/scratch.h>
#include "include/flash/entry.h"

// Include flash ROM
uint8_t __code __at(FLASH_OFFSET) flash_rom[] = {
    #include <flash.h>
};

static void flash_api(uint32_t addr, uint8_t * data, uint32_t length, uint8_t command) {
    // Use DMA mapping to copy flash ROM to scratch ROM
    SCARH = 0x80;
    SCARL = (uint8_t)(FLASH_OFFSET);
    SCARM = (uint8_t)(FLASH_OFFSET >> 8);
    SCARH = 0;

    // Jump to flash ROM
    flash_entry(addr, data, length, command);

    // Disable scratch ROM
    SCARH = 0x07;
}

void flash_read(uint32_t addr, __xdata uint8_t * data, uint32_t length) {
    flash_api(addr, data, length, FLASH_COMMAND_READ);
}

uint32_t flash_read_u32(uint32_t addr) {
    uint32_t data;

    flash_api(addr, (uint8_t *)&data, sizeof(data), FLASH_COMMAND_READ);

    return data;
}

uint16_t flash_read_u16(uint32_t addr) {
    uint16_t data;

    flash_api(addr, (uint8_t *)&data, sizeof(data), FLASH_COMMAND_READ);

    return data;
}

uint8_t flash_read_u8(uint32_t addr) {
    uint8_t data;

    flash_api(addr, &data, sizeof(data), FLASH_COMMAND_READ);

    return data;
}

void flash_write(uint32_t addr, __xdata uint8_t *data, uint32_t length) {
    flash_api(addr, data, length, FLASH_COMMAND_WRITE);
}

void flash_write_u32(uint32_t addr, uint32_t data) {
    flash_api(addr, (uint8_t *)&data, sizeof(data), FLASH_COMMAND_WRITE);
}

void flash_write_u16(uint32_t addr, uint16_t data) {
    flash_api(addr, (uint8_t *)&data, sizeof(data), FLASH_COMMAND_WRITE);
}

void flash_write_u8(uint32_t addr, uint8_t data) {
    flash_api(addr, &data, sizeof(data), FLASH_COMMAND_WRITE);
}

void flash_erase(uint32_t addr) {
    flash_api(addr, NULL, 0, FLASH_COMMAND_ERASE_1K);
}
