// SPDX-License-Identifier: GPL-3.0-only

#include <stdbool.h>

#include <ec/ps2.h>

#define PS2(NUM) { \
    .control = &PSCTL ## NUM, \
    .interrupt = &PSINT ## NUM, \
    .status = &PSSTS ## NUM, \
    .data = &PSDAT ## NUM, \
}

#define PS2_TIMEOUT 10000

#define PSSTS_TIMEOUT_ERR (1 << 6)
#define PSSTS_FRAME_ERR (1 << 5)
#define PSSTS_PARITY_ERR (1 << 4)
#define PSSTS_ALL_ERR (PSSTS_TIMEOUT_ERR | PSSTS_FRAME_ERR | PSSTS_PARITY_ERR)
#define PSSTS_DONE (1 << 3)

struct Ps2 __code PS2_1 = PS2(1);
struct Ps2 __code PS2_2 = PS2(2);
struct Ps2 __code PS2_3 = PS2(3);

void ps2_reset(struct Ps2 * ps2) {
    // Reset interface to defaults
    *(ps2->control) = 1;
    // Clear status
    *(ps2->status) = *(ps2->status);
}

static int ps2_transaction(struct Ps2 * ps2, uint8_t * data, int length, bool read) {
    int i;
    for (i = 0; i < length; i++) {
        if (read) {
            // Begin read
            *(ps2->control) = 0x07;
        } else {
            // Begin write
            *(ps2->control) = 0x0D;
            *(ps2->data) = data[i];
            // Pull data line low
            *(ps2->control) = 0x0C;
            // Pull clock line high
            *(ps2->control) = 0x0E;
        }

        uint32_t timeout;
        for (timeout = PS2_TIMEOUT; timeout > 0; timeout--) {
            uint8_t status = *(ps2->status);
            // If an error happened, clear status and return the error
            if (status & PSSTS_ALL_ERR) {
                ps2_reset(ps2);
                return -(int)status;
            }
            // If transaction is done, break
            if (status & PSSTS_DONE) {
                break;
            }
        }
        // If a timeout happened, return the error
        if (timeout == 0) {
            ps2_reset(ps2);
            return -0x1000;
        }
        if (read) {
            data[i] = *(ps2->data);
        }
        // Set interface to defaults
        ps2_reset(ps2);
    }

    return i;
}

int ps2_read(struct Ps2 * ps2, uint8_t * data, int length) {
    return ps2_transaction(ps2, data, length, true);
}

int ps2_write(struct Ps2 * ps2, uint8_t * data, int length) {
    return ps2_transaction(ps2, data, length, false);
}
