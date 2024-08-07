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
    // Enable/disable no input mode
    CMD_SET_NO_INPUT = 19,
    // Get security state
    CMD_SECURITY_GET = 20,
    // Set security state
    CMD_SECURITY_SET = 21,
    // Get USB-C mux info (for coreboot)
    CMD_USBC_MUX_INFO = 22,
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

enum SecurityState {
    // Default value, flashing is prevented, cannot be set with CMD_SECURITY_SET
    SECURITY_STATE_LOCK = 0,
    // Flashing is allowed, cannot be set with CMD_SECURITY_SET
    SECURITY_STATE_UNLOCK = 1,
    // Flashing will be prevented on the next reboot
    SECURITY_STATE_PREPARE_LOCK = 2,
    // Flashing will be allowed on the next reboot
    SECURITY_STATE_PREPARE_UNLOCK = 3,
};

enum UsbcMuxInfoFlags {
    // DisplayPort connected if set
    CMD_USBC_MUX_INFO_DP = BIT(0),
    // USB connected if set
    CMD_USBC_MUX_INFO_USB = BIT(1),
    // Active cable if set, passive if not set
    CMD_USBC_MUX_INFO_CABLE = BIT(2),
    // Polarity of device, flipped if set, normal if not set
    CMD_USBC_MUX_INFO_POLARITY = BIT(3),
    // HPD level assert
    CMD_USBC_MUX_INFO_HPD_LVL = BIT(4),
    // HPD IRQ assert
    CMD_USBC_MUX_INFO_HPD_IRQ = BIT(5),
    // UFP if set, DFP if not set
    CMD_USBC_MUX_INFO_UFP = BIT(6),
    // Debug accessory if set
    CMD_USBC_MUX_INFO_DBG_ACC = BIT(7),
    // DP pin mode A
    CMD_USBC_MUX_INFO_DP_MODE_A = BIT(8),
    // DP pin mode B
    CMD_USBC_MUX_INFO_DP_MODE_B = BIT(9),
    // DP pin mode C
    CMD_USBC_MUX_INFO_DP_MODE_C = BIT(10),
    // DP pin mode D
    CMD_USBC_MUX_INFO_DP_MODE_D = BIT(11),
    // DP pin mode E
    CMD_USBC_MUX_INFO_DP_MODE_E = BIT(12),
    // DP pin mode F
    CMD_USBC_MUX_INFO_DP_MODE_F = BIT(13),
};

#endif // _COMMON_COMMAND_H
