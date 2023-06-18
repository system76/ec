// SPDX-License-Identifier: GPL-3.0-only

#include <common/i2c.h>

int16_t i2c_recv(struct I2C *i2c, uint8_t addr, uint8_t *data, uint16_t length) __reentrant {
    int16_t res = 0;

    res = i2c_start(i2c, addr, true);
    if (res < 0)
        return res;

    res = i2c_read(i2c, data, length);
    if (res < 0)
        return res;

    i2c_stop(i2c);

    return res;
}

int16_t i2c_send(struct I2C *i2c, uint8_t addr, uint8_t *data, uint16_t length) __reentrant {
    int16_t res = 0;

    res = i2c_start(i2c, addr, false);
    if (res < 0)
        return res;

    res = i2c_write(i2c, data, length);
    if (res < 0)
        return res;

    i2c_stop(i2c);

    return res;
}

int16_t i2c_get(struct I2C *i2c, uint8_t addr, uint8_t reg, uint8_t *data, uint16_t length)
    __reentrant {
    int16_t res = 0;

    res = i2c_start(i2c, addr, false);
    if (res < 0)
        return res;

    res = i2c_write(i2c, &reg, 1);
    if (res < 0)
        return res;

    return i2c_recv(i2c, addr, data, length);
}

int16_t i2c_set(struct I2C *i2c, uint8_t addr, uint8_t reg, uint8_t *data, uint16_t length)
    __reentrant {
    int16_t res = 0;

    res = i2c_start(i2c, addr, false);
    if (res < 0)
        return res;

    res = i2c_write(i2c, &reg, 1);
    if (res < 0)
        return res;

    return i2c_send(i2c, addr, data, length);
}
