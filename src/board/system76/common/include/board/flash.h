/*
 * Copyright (C) 2020 Evan Lojewski
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef _BOARD_FLASH_H
#define _BOARD_FLASH_H

#include <stdint.h>

/** \cond INTERNAL
 * Internal defines
 */
#define FLASH_COMMAND_READ      (0x0)
#define FLASH_COMMAND_WRITE     (0x1)
#define FLASH_COMMAND_ERASE_1K  (0x2)
/** \endcond */

/**
 * Read data from flash to the specified buffer.
 *
 * \param[in]  addr   The flash address to read.
 * \param[out] data   The memory area to copy to.
 * \param[in]  length The number of bytes to copy.
 */
void flash_read(uint32_t addr, __xdata uint8_t * data, uint32_t length);

/**
 * Read a single byte from flash.
 *
 * \param[in] addr   The flash address to read.
 *
 * \return The value read from flash.
 */
uint8_t flash_read_u8(uint32_t addr);

/**
 * Read two bytes from flash.
 *
 * \param[in] addr   The flash address to read.
 *
 * \return The value read from flash.
 */
uint16_t flash_read_u16(uint32_t addr);

/**
 * Read four bytes from flash.
 *
 * \param[in] addr   The flash address to read.
 *
 * \return The value read from flash.
 */
uint32_t flash_read_u32(uint32_t addr);

/**
 * Write data to flash from the specified buffer.
 *
 * \param[in] addr   The flash address to read.
 * \param[in] data   The memory area to copy from.
 * \param[in] length The number of bytes to copy.
 */
void flash_write(uint32_t addr, __xdata uint8_t * data, uint32_t length);

/**
 * Write a single byte to flash.
 *
 * \param[in] addr   The flash address to read.
 * \param[in] data   The value to write to flash.
 */
void flash_write_u8(uint32_t addr, uint8_t data);

/**
 * Write two bytes to flash.
 *
 * \param[in] addr   The flash address to read.
 * \param[in] data   The value to write to flash.
 */
void flash_write_u16(uint32_t addr, uint16_t data);

/**
 * Write two bytes to flash.
 *
 * \param[in] addr   The flash address to read.
 * \param[in] data   The value to write to flash.
 */
void flash_write_u32(uint32_t addr, uint32_t data);

/**
 * Erase a 1K block of flash.
 *
 * \param[in] addr  The flash address contained in the 1K block.
 */
void flash_erase(uint32_t addr);

#endif // _BOARD_FLASH_H
