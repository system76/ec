/*
 * Copyright (C) 2020 Evan Lojewski
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <stdint.h>
#include <board/flash.h>
#include <flash/entry.h>

// EC indirect flash access
volatile uint8_t __xdata __at(0x103B) ECINDAR0;
volatile uint8_t __xdata __at(0x103C) ECINDAR1;
volatile uint8_t __xdata __at(0x103D) ECINDAR2;
volatile uint8_t __xdata __at(0x103E) ECINDAR3;
volatile uint8_t __xdata __at(0x103F) ECINDDR;

#define SPI_DEVICE                  (0x70)
#define SPI_FOLLOW_MODE             (0x0F)
#define SPI_CHIP_SELECT             (0xFD)
#define SPI_CHIP_DESELECT           (0xFE)

#define SPI_READ_STATUS_COMMAND     (0x05)
#define SPI_READ_COMMAND            (0x0B)
#define SPI_WRITE_COMMAND           (0x02)
#define SPI_WRITE_ENABLE_COMMAND    (0x06)
#define SPI_READ_STATUS_COMMAND     (0x05)

#define SPI_ERASE_SECTOR_COMMAND    (0xD7)

#define SPI_STATUS_WIP              (0x01)

void flash_enter_follow_mode(void);
void flash_exit_follow_mode(void);
void flash_wait(void);
void flash_write_enable(void);

/**
 * Main flash API entry point.
 *
 * NOTE: This *must* be the first function in this file to ensure that it is placed
 *          first in the resulting binary. This is required to ensure that address
 *          matches the address (FLASH_OFFSET) for flash_entry in wrapper.c.
 * NOTE: __reentrant so that parameters and temperary vairables are placed on the
 *          stack, ensuring the main application __data variables are not stomped on.
 * NOTE: __critical to ensure interrupts are disabled. This does mean that interrupt
 *          such as the timer will be block until flash acccess is complete
 */
void flash_entry(uint32_t addr, uint8_t * data, uint32_t length, uint8_t command) __reentrant __critical {
    // Only allow access from 64KB to 128KB.
    if ((addr < 0x10000)
    || (length > 0x10000)
    || ((addr + length) > 0x20000))
        return;

    if (command == FLASH_COMMAND_READ) {
        while (length) {
            // Fast read.
            ECINDAR3 = SPI_DEVICE;
            ECINDAR2 = addr >> 16;
            ECINDAR1 = addr >> 8;
            ECINDAR0 = addr;

            *data = ECINDDR;

            addr++;
            data++;
            length--;
        }
    } else if (command == FLASH_COMMAND_WRITE) {
        flash_enter_follow_mode();

        while (length) {
            // Note, this is the slow way to do it, but it's simple and all bytes are written properly.
            flash_write_enable();

            // Select the device
            ECINDAR1 = SPI_CHIP_SELECT;

            // Send write command
            ECINDDR = SPI_WRITE_COMMAND;
            ECINDDR = addr >> 16;
            ECINDDR = addr >> 8;
            ECINDDR = addr;

            ECINDDR = *data;

            addr++;
            data++;
            length--;

            // Deselect
            ECINDAR1 = SPI_CHIP_DESELECT;
            ECINDDR  = 0x00;


            // Wait WIP to be cleared
            flash_wait();
        }

        flash_exit_follow_mode();
    } else if (command == FLASH_COMMAND_ERASE_1K) {
        flash_enter_follow_mode();

        flash_write_enable();

        // Select the device
        ECINDAR1 = SPI_CHIP_SELECT;

        // Send erase command
        ECINDDR = SPI_ERASE_SECTOR_COMMAND;
        ECINDDR = addr >> 16;
        ECINDDR = addr >> 8;
        ECINDDR = addr;

        // Deselect
        ECINDAR1 = SPI_CHIP_DESELECT;
        ECINDDR  = 0x00;

        // Wait WIP to be cleared
        flash_wait();

        flash_exit_follow_mode();
    }
}

void flash_enter_follow_mode(void) {
    // Enter follow mode.
    ECINDAR3 = SPI_FOLLOW_MODE | SPI_DEVICE;
    ECINDAR2 = 0xFF;
    ECINDAR1 = SPI_CHIP_DESELECT;
    ECINDAR0 = 0x00;
    ECINDDR = 0x00;
}

void flash_exit_follow_mode(void) {
    // Exit follow mode
    ECINDAR3 = SPI_DEVICE;
    ECINDAR2 = 0;
    ECINDAR1 = 0;
    ECINDAR0 = 0;
}

void flash_wait(void) {
    uint8_t status;

    do {
        // Select the device
        ECINDAR1 = SPI_CHIP_SELECT;

        // Send command
        ECINDDR = SPI_READ_STATUS_COMMAND;

        // read status
        status = ECINDDR;

        // Deselect
        ECINDAR1 = SPI_CHIP_DESELECT;
        ECINDDR  = 0x00;
    } while(status & SPI_STATUS_WIP);
}

void flash_write_enable(void) {
    // Select the device
    ECINDAR1 = SPI_CHIP_SELECT;

    // Send device id command
    ECINDDR = SPI_WRITE_ENABLE_COMMAND;

    // Deselect
    ECINDAR1 = SPI_CHIP_DESELECT;
    ECINDDR  = 0x00;
}
