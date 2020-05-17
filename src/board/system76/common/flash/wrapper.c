/*
 * Copyright (C) 2020 Evan Lojewski
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <stdint.h>
#include <stddef.h>
#include <board/flash.h>
#include <common/ec.h>

#if __EC__ == it5570e
// IT5570: SCAR0 is stored in processor cache, not in xram
volatile uint8_t __xdata __at(0x1040) SCAR0L;
volatile uint8_t __xdata __at(0x1041) SCAR0M;
volatile uint8_t __xdata __at(0x1042) SCAR0H;

#define SCARL SCAR0L
#define SCARM SCAR0M
#define SCARH SCAR0H

#elif __EC__ == it8587e
// IT8587: SCAR1 is in xram at 0x800-0xC00
volatile uint8_t __xdata __at(0x1043) SCAR1L;
volatile uint8_t __xdata __at(0x1044) SCAR1M;
volatile uint8_t __xdata __at(0x1045) SCAR1H;

#define SCARL SCAR1L
#define SCARM SCAR1M
#define SCARH SCAR1H

#else
#error Unknown EC
#endif


void flash_entry(uint32_t addr, uint8_t * data, uint8_t length, uint8_t command) __reentrant;

// Include flash ROM
uint8_t __code __at(FLASH_OFFSET) flash_rom[] = {
    #include <flash.h>
};

void flash_init(void) {
    // Use DMA mapping to copy flash data
    SCARH = 0x80;
    SCARL = (uint8_t)(FLASH_OFFSET);
    SCARM = (uint8_t)(FLASH_OFFSET >> 8);
    SCARH = 0;
}

void flash_read(uint32_t addr, __xdata uint8_t * data, uint8_t length) {
    flash_entry(addr, data, length, FLASH_COMMAND_READ);
}

uint32_t flash_read_u32(uint32_t addr) {
    uint32_t data;

    flash_entry(addr, (uint8_t *)&data, sizeof(data), FLASH_COMMAND_READ);

    return data;
}

uint16_t flash_read_u16(uint32_t addr) {
    uint16_t data;

    flash_entry(addr, (uint8_t *)&data, sizeof(data), FLASH_COMMAND_READ);

    return data;
}

uint8_t flash_read_u8(uint32_t addr) {
    uint8_t data;

    flash_entry(addr, &data, sizeof(data), FLASH_COMMAND_READ);

    return data;
}

void flash_write(uint32_t addr, __xdata uint8_t *data, uint8_t length) {
    flash_entry(addr, data, length, FLASH_COMMAND_WRITE);
}

void flash_write_u32(uint32_t addr, uint32_t data) {
    flash_entry(addr, (uint8_t *)&data, sizeof(data), FLASH_COMMAND_WRITE);
}

void flash_write_u16(uint32_t addr, uint16_t data) {
    flash_entry(addr, (uint8_t *)&data, sizeof(data), FLASH_COMMAND_WRITE);
}

void flash_write_u8(uint32_t addr, uint8_t data) {
    flash_entry(addr, &data, sizeof(data), FLASH_COMMAND_WRITE);
}

void flash_erase(uint32_t addr) {
    flash_entry(addr, NULL, 0, FLASH_COMMAND_ERASE_1K);
}
