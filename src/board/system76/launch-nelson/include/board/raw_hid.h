// SPDX-License-Identifier: GPL-3.0-only

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifndef _BOARD_HID_H
#define _BOARD_HID_H

void raw_hid_init(void);
void raw_hid_receive(uint8_t *data, uint8_t length);
void raw_hid_send(uint8_t *data, uint8_t length);

#endif // _BOARD_HID_H
