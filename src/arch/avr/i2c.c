// SPDX-License-Identifier: GPL-3.0-only

#include <stdio.h>
#include <avr/io.h>
#include <util/twi.h>

#include <board/cpu.h>
#include <common/i2c.h>
#include <common/macro.h>

#define TIMEOUT (F_CPU / 1000)

int16_t i2c_start(struct I2C *i2c, uint8_t addr, bool read) {
    uint32_t count;

    // reset TWI control register
    TWCR = 0;
    // transmit START condition
    TWCR = BIT(TWINT) | BIT(TWSTA) | BIT(TWEN);
    // wait for end of transmission
    count = TIMEOUT;
    while (!(TWCR & BIT(TWINT)) && count > 0)
        count -= 1;
    if (count == 0)
        return -1;

    // check if the start condition was successfully transmitted
    if ((TWSR & 0xF8) != TW_START)
        return -1;

    // load slave addr into data register
    TWDR = ((addr << 1) | read);
    // start transmission of addr
    TWCR = BIT(TWINT) | BIT(TWEN);
    // wait for end of transmission
    count = TIMEOUT;
    while (!(TWCR & BIT(TWINT)) && count > 0)
        count -= 1;
    if (count == 0)
        return -1;

    // check if the device has acknowledged the READ / WRITE mode
    uint8_t twst = TW_STATUS & 0xF8;
    if ((twst != TW_MT_SLA_ACK) && (twst != TW_MR_SLA_ACK))
        return -1;

    return 0;
}

void i2c_stop(struct I2C *i2c) {
    // transmit STOP condition
    TWCR = BIT(TWINT) | BIT(TWEN) | BIT(TWSTO);
}

int16_t i2c_write(struct I2C *i2c, uint8_t *data, uint16_t length) {
    uint16_t i;
    for (i = 0; i < length; i++) {
        // load data into data register
        TWDR = data[i];
        // start transmission of data
        TWCR = BIT(TWINT) | BIT(TWEN);
        // wait for end of transmission
        uint32_t count = TIMEOUT;
        while (!(TWCR & BIT(TWINT)) && count > 0)
            count -= 1;
        // timed out
        if (count == 0)
            return -1;
        // failed to receive ack
        if ((TWSR & 0xF8) != TW_MT_DATA_ACK)
            return -1;
    }

    return i;
}

int16_t i2c_read(struct I2C *i2c, uint8_t *data, uint16_t length) {
    uint16_t i;
    for (i = 0; i < length; i++) {
        if ((i + 1) < length) {
            // start TWI module and acknowledge data after reception
            TWCR = BIT(TWINT) | BIT(TWEN) | BIT(TWEA);
        } else {
            // start receiving without acknowledging reception
            TWCR = BIT(TWINT) | BIT(TWEN);
        }
        // wait for end of transmission
        uint32_t count = TIMEOUT;
        while (!(TWCR & BIT(TWINT)) && count > 0)
            count -= 1;
        if (count == 0)
            return -1;
        // return received data from TWDR
        data[i] = TWDR;
    }

    return i;
}
