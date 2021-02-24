// SPDX-License-Identifier: GPL-3.0-only

#ifndef _COMMON_COMMAND_H
#define _COMMON_COMMAND_H

#include <common/macro.h>

enum Command {
    // Indicates that EC is ready to accept commands
    CMD_NONE = 0,
    // Probe for System76 EC protocol
    CMD_PROBE = 1,
    // Read board string
    CMD_BOARD = 2,
    // Read version string
    CMD_VERSION = 3,
    // Write bytes to console
    CMD_PRINT = 4,
    // Access SPI chip
    CMD_SPI = 5,
    // Reset EC
    CMD_RESET = 6,
    // Get fan speeds
    CMD_FAN_GET = 7,
    // Set fan speeds
    CMD_FAN_SET = 8,
    // Get keyboard map index
    CMD_KEYMAP_GET = 9,
    // Set keyboard map index
    CMD_KEYMAP_SET = 10,
    // Get LED value by index
    CMD_LED_GET_VALUE = 11,
    // Set LED value by index
    CMD_LED_SET_VALUE = 12,
    // Get LED color by index
    CMD_LED_GET_COLOR = 13,
    // Set LED color by index
    CMD_LED_SET_COLOR = 14,
    // Get LED matrix mode and speed
    CMD_LED_GET_MODE = 15,
    // Set LED matrix mode and speed
    CMD_LED_SET_MODE = 16,
    // Get key matrix state
    CMD_MATRIX_GET = 17,
    // Save LED settings to ROM
    CMD_LED_SAVE = 18,
    //TODO
};

enum Result {
    // Command executed successfully
    RES_OK = 0,
    // Command failed with generic error
    RES_ERR = 1,
    //TODO
};

enum CommandSpiFlag {
    // Read from SPI chip if set, write otherwise
    CMD_SPI_FLAG_READ = BIT(0),
    // Disable SPI chip after executing command
    CMD_SPI_FLAG_DISABLE = BIT(1),
    // Run firmware from scratch RAM if necessary
    CMD_SPI_FLAG_SCRATCH = BIT(2),
    // Write to backup ROM instead
    CMD_SPI_FLAG_BACKUP = BIT(3),
};

#define CMD_LED_INDEX_ALL 0xFF

#endif // _COMMON_COMMAND_H
