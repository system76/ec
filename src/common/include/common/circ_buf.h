// SPDX-License-Identifier: GPL-3.0-only


#ifndef _COMMON_CIRC_BUF_H
#define _COMMON_CIRC_BUF_H

#include <stdint.h>

// Generic circular buffer using a pointer for the buffer.
struct circ_buf {
    uint8_t *buf;
    uint8_t head;
    uint8_t tail;
};

// Circular buffer using a defined size for the buffer.
#define DECL_CIRC_BUF(name, size) \
    struct { \
        uint8_t buf[size]; \
        uint8_t head; \
        uint8_t tail; \
    } name

#endif // _COMMON_CIRC_BUF_H
