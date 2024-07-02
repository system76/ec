// SPDX-License-Identifier: GPL-3.0-only

#ifndef _BOARD_DGPU_H
#define _BOARD_DGPU_H

#include <stdbool.h>
#include <stdint.h>

#if CONFIG_HAVE_DGPU

extern int16_t dgpu_temp;

void dgpu_init(void);
bool dgpu_get_temp(int16_t *const data);
void dgpu_read_temp(void);

#else

static inline void dgpu_init(void) {}

static inline bool dgpu_get_temp(int16_t *const data) {
    *data = 0;
    return true;
}

static inline void dgpu_read_temp(void) {}

#endif // CONFIG_HAVE_DGPU

#endif // _BOARD_DGPU_H
