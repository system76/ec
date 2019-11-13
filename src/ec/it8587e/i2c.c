#include <stdbool.h>

#include <board/cpu.h>
#include <common/i2c.h>
#include <ec/smbus.h>

//TODO: find best value
#define TIMEOUT (F_CPU/1000)

#define HOSTA_BYTE_DONE (1 << 7)
#define HOSTA_TIMEOUT (1 << 6)
#define HOSTA_NACK (1 << 5)
#define HOSTA_FAIL (1 << 4)
#define HOSTA_BUS_ERR (1 << 3)
#define HOSTA_DEV_ERR (1 << 2)
#define HOSTA_FINISH (1 << 1)
#define HOSTA_BUSY (1 << 0)
#define HOSTA_ERR (HOSTA_TIMEOUT | HOSTA_NACK | HOSTA_FAIL | HOSTA_BUS_ERR | HOSTA_DEV_ERR)

void i2c_reset(bool kill) {
    // Set kill bit
    if (kill) HOCTLA |= (1 << 1);
    // Wait for host to finish
    while (HOSTAA & HOSTA_BUSY) {}
    // Clear status register
    HOSTAA = HOSTAA;
    // Clear current command
    HOCTLA = 0;
    // Disable host interface
    HOCTL2A = 0;
}

int i2c_start(uint8_t addr, bool read) {
    // If we are already in a transaction
    if (HOSTAA & HOSTA_BYTE_DONE) {
        // If we are switching direction
        if ((TRASLAA & 1) != read) {
            // If we are switching to read mode
            if (read) {
                // Enable direction switch
                HOCTL2A |= (1 << 3) | (1 << 2);

                // Clear status to start switch process
                HOSTAA = HOSTAA;
            } else {
                // Unsupported!
                i2c_reset(true);
                return -1;
            }
        }
    } else {
        // Enable host controller with i2c compatibility
        HOCTL2A = (1 << 1) | 1;

        // Set address
        TRASLAA = (addr << 1) | read;
    }

	return 0;
}

void i2c_stop(void) {
    // Disable i2c compatibility
    HOCTL2A &= ~(1 << 1);
    // Clear status
    HOSTAA = HOSTAA;

    i2c_reset(false);
}

static int i2c_transaction(uint8_t * data, int length, bool read) {
    int i;
    for (i = 0; i < length; i++) {
        if (read) {
            // If last byte
            if ((i + 1) == length) {
                // Set last byte bit
                HOCTLA |= (1 << 5);
            }
        } else {
            // Write byte
            HOBDBA = data[i];
        }

        // If we are already in a transaction
        if (HOSTAA & HOSTA_BYTE_DONE) {
            // Clear status to process next byte
            HOSTAA = HOSTAA;
        } else
        // If we are waiting on direction switch
        if (HOCTL2A & (1 << 2)) {
            // Complete direction switch
            HOCTL2A &= ~(1 << 2);
        } else {
            // Start new transaction
            HOCTLA = (1 << 6) | (0b111 << 2);
        }

        // Wait for byte done, timeout, or error
        // uint32_t timeout;
        // for(timeout = TIMEOUT; timeout > 0; timeout--) {
        for(;;) {
            uint8_t status = HOSTAA;
            if (status & HOSTA_BYTE_DONE) {
                // If byte is finished, break
                break;
            } else if (status & HOSTA_ERR) {
                // If error occured, kill transaction and return error
                i2c_reset(true);
                return -(int)(status & HOSTA_ERR);
            }
        }
        // if (timeout == 0) {
        //     // If timeout occured, kill transaction and return error
        //     i2c_reset(true);
        //     return -1;
        // }

        if (read) {
            // Read byte
            data[i] = HOBDBA;
        }
    }

    return i;
}

int i2c_read(uint8_t * data, int length) {
    return i2c_transaction(data, length, true);
}

int i2c_write(uint8_t * data, int length) {
    return i2c_transaction(data, length, false);
}
