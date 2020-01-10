#include <stdbool.h>

#include <common/i2c.h>
#include <ec/smbus.h>

//TODO: find best value
#define I2C_TIMEOUT 10000

#define HOSTA HOSTAE
#define HOCTL HOCTLE
#define HOCTL2 HOCTL2E
#define HOBDB HOBDBE
#define TRASLA TRASLAE

void i2c_reset(bool kill) {
    if (HOSTA & HOSTA_BUSY) {
        // Set kill bit
        if (kill) HOCTL |= (1 << 1);
        // Wait for host to finish
        while (HOSTA & HOSTA_BUSY) {}
    }
    // Clear status register
    HOSTA = HOSTA;
    // Clear current command
    HOCTL = 0;
    // Disable host interface
    HOCTL2 = 0;
}

int i2c_start(uint8_t addr, bool read) {
    // If we are already in a transaction
    if (HOSTA & HOSTA_BYTE_DONE) {
        // If we are switching direction
        if ((TRASLA & 1) != read) {
            // If we are switching to read mode
            if (read) {
                // Enable direction switch
                HOCTL2 |= (1 << 3) | (1 << 2);
            } else {
                // Unsupported!
                i2c_reset(true);
                return -1;
            }
        }
    } else {
        i2c_reset(true);

        // Enable host controller with i2c compatibility
        HOCTL2 = (1 << 1) | 1;

        // Set address
        TRASLA = (addr << 1) | read;
    }

	return 0;
}

void i2c_stop(void) {
    // Disable i2c compatibility
    HOCTL2 &= ~(1 << 1);
    // Clear status
    HOSTA = HOSTA;

    i2c_reset(false);
}

static int i2c_transaction(uint8_t * data, int length, bool read) {
    int i;
    for (i = 0; i < length; i++) {
        if (read) {
            // If last byte
            if ((i + 1) == length) {
                // Set last byte bit
                HOCTL |= (1 << 5);
            }
        } else {
            // Write byte
            HOBDB = data[i];
        }

        // If we are already in a transaction
        if (HOSTA & HOSTA_BYTE_DONE) {
            // Clear status to process next byte
            HOSTA = HOSTA;
        } else {
            // Start new transaction
            HOCTL = (1 << 6) | (0b111 << 2);
        }

        // If we are waiting on direction switch
        if (HOCTL2 & (1 << 2)) {
            // Complete direction switch
            HOCTL2 &= ~(1 << 2);
        }

        // Wait for byte done, timeout, or error
        uint8_t status;
        uint32_t timeout = I2C_TIMEOUT;
        for(timeout = I2C_TIMEOUT; timeout > 0; timeout--) {
            status = HOSTA;
            // If error occured, kill transaction and return error
            if (status & HOSTA_ERR) {
                i2c_reset(true);
                return -(int)(status);
            } else
            // If byte done, break
            if (status & HOSTA_BYTE_DONE) {
                break;
            }
        }
        // If timeout occured, kill transaction and return error
        if (timeout == 0) {
            i2c_reset(true);
            return -(0x1000 | (int)status);
        }

        if (read) {
            // Read byte
            data[i] = HOBDB;
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
