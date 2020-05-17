// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef _FLASH_ENTRY_H
#define _FLASH_ENTRY_H

void flash_entry(uint32_t addr, uint8_t * data, uint32_t length, uint8_t command) __reentrant;

#endif // _FLASH_ENTRY_H
