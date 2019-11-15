#include <stdbool.h>

#include <board/cpu.h>
#include <ec/ps2.h>

#define PS2(NUM) { \
    .control = &PSCTL ## NUM, \
    .interrupt = &PSINT ## NUM, \
    .status = &PSSTS ## NUM, \
    .data = &PSDAT ## NUM, \
}

#define TIMEOUT (F_CPU/1000)

#define PSSTS_TIMEOUT_ERR (1 << 6)
#define PSSTS_FRAME_ERR (1 << 5)
#define PSSTS_PARITY_ERR (1 << 4)
#define PSSTS_ALL_ERR (PSSTS_TIMEOUT_ERR | PSSTS_FRAME_ERR | PSSTS_PARITY_ERR)
#define PSSTS_DONE (1 << 3)

struct Ps2 __code PS2_1 = PS2(1);
struct Ps2 __code PS2_2 = PS2(2);
struct Ps2 __code PS2_3 = PS2(3);

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
        for (timeout = TIMEOUT; timeout > 0; timeout--) {
            uint8_t status = *(ps2->status);
            // If an error happened, clear status and return the error
            if (status & PSSTS_ALL_ERR) {
                *(ps2->status) = status;
                return -(int)status;
            }
            // If transaction is done, break
            if (status & PSSTS_DONE) {
                break;
            }
        }
        // If a timeout happened, return the error
        if (timeout == 0) {
            return -1;
        }
        if (read) {
            data[i] = *(ps2->data);
        }
    }

    return i;
}

int ps2_read(struct Ps2 * ps2, uint8_t * data, int length) {
    return ps2_transaction(ps2, data, length, true);
}

int ps2_write(struct Ps2 * ps2, uint8_t * data, int length) {
    return ps2_transaction(ps2, data, length, false);
}
