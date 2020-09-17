// SPDX-License-Identifier: GPL-3.0-only

#ifndef _BOARD_PARALLEL_H
#define _BOARD_PARALLEL_H

#include <stdbool.h>
#include <stdint.h>

extern bool parallel_debug;
bool parallel_init(void);
int parallel_write(uint8_t * data, int length);

#endif // _BOARD_PARALLEL_H
