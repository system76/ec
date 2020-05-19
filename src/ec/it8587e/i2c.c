// SPDX-License-Identifier: GPL-3.0-only

#include <stdbool.h>

#include <common/i2c.h>
#include <ec/smbus.h>

//TODO: find best value
#define I2C_TIMEOUT 10000

struct I2C {
    volatile uint8_t * hosta;
    volatile uint8_t * hoctl;
    volatile uint8_t * hoctl2;
    volatile uint8_t * hobdb;
    volatile uint8_t * trasla;
};

struct I2C __code I2C_0 = {
    .hosta = HOSTAA,
    .hoctl = HOCTLA,
    .hoctl2 = HOCTL2A,
    .hobdb = HOBDBA,
    .trasla = TRASLAA,
};

struct I2C __code I2C_1 = {
    .hosta = HOSTAB,
    .hoctl = HOCTLB,
    .hoctl2 = HOCTL2B,
    .hobdb = HOBDBB,
    .trasla = TRASLAB,
};

void i2c_reset(struct I2C * i2c, bool kill) {
    if (*(i2c->hosta) & HOSTA_BUSY) {
        // Set kill bit
        if (kill) *(i2c->hoctl) |= (1 << 1);
        // Wait for host to finish
        while (*(i2c->hosta) & HOSTA_BUSY) {}
    }
    // Clear status register
    *(i2c->hosta) = *(i2c->hosta);
    // Clear current command
    *(i2c->hoctl) = 0;
    // Disable host interface
    *(i2c->hoctl2) = 0;
}

int i2c_start(struct I2C * i2c, uint8_t addr, bool read) __reentrant {
    // If we are already in a transaction
    if (*(i2c->hosta) & HOSTA_BYTE_DONE) {
        // If we are switching direction
        if ((*(i2c->trasla) & 1) != read) {
            // If we are switching to read mode
            if (read) {
                // Enable direction switch
                *(i2c->hoctl2) |= (1 << 3) | (1 << 2);
            } else {
                // Unsupported!
                i2c_reset(i2c, true);
                return -1;
            }
        }
    } else {
        i2c_reset(i2c, true);

        // Enable host controller with i2c compatibility
        *(i2c->hoctl2) = (1 << 1) | 1;

        // Set address
        *(i2c->trasla) = (addr << 1) | read;
    }

	return 0;
}

void i2c_stop(struct I2C * i2c) {
    // Disable i2c compatibility
    *(i2c->hoctl2) &= ~(1 << 1);
    // Clear status
    *(i2c->hosta) = *(i2c->hosta);

    i2c_reset(i2c, false);
}

static int i2c_transaction(struct I2C * i2c, uint8_t * data, int length, bool read) {
    int i;
    for (i = 0; i < length; i++) {
        if (read) {
            // If last byte
            if ((i + 1) == length) {
                // Set last byte bit
                *(i2c->hoctl) |= (1 << 5);
            }
        } else {
            // Write byte
            *(i2c->hobdb) = data[i];
        }

        // If we are already in a transaction
        if (*(i2c->hosta) & HOSTA_BYTE_DONE) {
            // Clear status to process next byte
            *(i2c->hosta) = *(i2c->hosta);
        } else {
            // Start new transaction
            *(i2c->hoctl) = (1 << 6) | (0b111 << 2);
        }

        // If we are waiting on direction switch
        if (*(i2c->hoctl2) & (1 << 2)) {
            // Complete direction switch
            *(i2c->hoctl2) &= ~(1 << 2);
        }

        // Wait for byte done, timeout, or error
        uint8_t status;
        uint32_t timeout = I2C_TIMEOUT;
        for(timeout = I2C_TIMEOUT; timeout > 0; timeout--) {
            status = *(i2c->hosta);
            // If error occured, kill transaction and return error
            if (status & HOSTA_ERR) {
                i2c_reset(i2c, true);
                return -(int)(status);
            } else
            // If byte done, break
            if (status & HOSTA_BYTE_DONE) {
                break;
            }
        }
        // If timeout occured, kill transaction and return error
        if (timeout == 0) {
            i2c_reset(i2c, true);
            return -(0x1000 | (int)status);
        }

        if (read) {
            // Read byte
            data[i] = *(i2c->hobdb);
        }
    }

    return i;
}

int i2c_read(struct I2C * i2c, uint8_t * data, int length) __reentrant {
    return i2c_transaction(i2c, data, length, true);
}

int i2c_write(struct I2C * i2c, uint8_t * data, int length) __reentrant {
    return i2c_transaction(i2c, data, length, false);
}
