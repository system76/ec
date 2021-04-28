// SPDX-License-Identifier: GPL-3.0-only

#include <string.h>
#include <board/cpu.h>
#include <util/delay.h>

#include <arch/gpio.h>
#include <board/raw_hid.h>
#include <common/command.h>
#include <common/version.h>

static struct Gpio relay = GPIO(B, 4);

void Jump_To_Bootloader(void);
static bool bootloader_reset = false;

#define HID_CMD 0
#define HID_RES 1
#define HID_DATA 2
#define HID_LEN 32

void raw_hid_init(void) {
    gpio_set(&relay, false);
    gpio_set_dir(&relay, true);
}

void raw_hid_receive(uint8_t* data, uint8_t length) {
    // Exit if length does not match
    if (length != HID_LEN) {
        return;
    }

    switch (data[HID_CMD]) {
        case CMD_PROBE:
            data[HID_DATA + 0] = 0x76;
            data[HID_DATA + 1] = 0xEC;
            data[HID_DATA + 2] = 0x01;
            data[HID_RES] = RES_OK;
            break;
        case CMD_BOARD:
            strncpy((char *)&data[HID_DATA], board(), length - HID_DATA);
            data[HID_RES] = RES_OK;
            break;
        case CMD_VERSION:
            strncpy((char *)&data[HID_DATA], version(), length - HID_DATA);
            data[HID_RES] = RES_OK;
            break;
        case CMD_RESET:
            bootloader_reset = true;
            data[HID_RES] = RES_OK;
            break;
        case CMD_LED_GET_VALUE:
            if (data[HID_DATA + 0] == 0) {
                data[HID_DATA + 1] = gpio_get(&relay);
                data[HID_DATA + 2] = 1;
                data[HID_RES] = RES_OK;
            } else {
                // LED not found
                data[HID_RES] = RES_ERR;
            }
            break;
        case CMD_LED_SET_VALUE:
            if (data[HID_DATA + 0] == 0) {
                gpio_set(&relay, data[HID_DATA + 1] > 0);
                data[HID_RES] = RES_OK;
            } else {
                // LED not found
                data[HID_RES] = RES_ERR;
            }
            break;
        default:
            // Command not found
            data[HID_RES] = RES_ERR;
            break;
    }

    // Mark command as finished
    data[HID_CMD] = CMD_NONE;

    raw_hid_send(data, length);

    if (bootloader_reset) {
        // Give host time to read response
        _delay_ms(100);
        // Jump to the bootloader
        Jump_To_Bootloader();
    }
}
