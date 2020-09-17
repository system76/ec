// SPDX-License-Identifier: GPL-3.0-only

#ifndef _COMMON_COMMAND_H
#define _COMMON_COMMAND_H

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
    CMD_SPI_FLAG_READ = (1 << 0),
    // Disable SPI chip after executing command
    CMD_SPI_FLAG_DISABLE = (1 << 1),
    // Run firmware from scratch RAM if necessary
    CMD_SPI_FLAG_SCRATCH = (1 << 2),
    // Write to backup ROM instead
    CMD_SPI_FLAG_BACKUP = (1 << 3),
};

#endif // _COMMON_COMMAND_H
