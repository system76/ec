#include <common/i2c.h>

int i2c_recv(uint8_t addr, uint8_t* data, int length) {
    int res = 0;

    res = i2c_start(addr, true);
    if (res < 0) return res;

    res = i2c_read(data, length);
    if (res < 0) return res;

	i2c_stop();

	return res;
}

int i2c_send(uint8_t addr, uint8_t* data, int length) {
    int res = 0;

    res = i2c_start(addr, false);
    if (res < 0) return res;

    res = i2c_write(data, length);
    if (res < 0) return res;

	i2c_stop();

	return res;
}

int i2c_get(uint8_t addr, uint8_t reg, uint8_t* data, int length) {
    int res = 0;

    res = i2c_start(addr, false);
    if (res < 0) return res;

    res = i2c_write(&reg, 1);
    if (res < 0) return res;

    return i2c_recv(addr, data, length);
}

int i2c_set(uint8_t addr, uint8_t reg, uint8_t* data, int length) {
    int res = 0;

    res = i2c_start(addr, false);
    if (res < 0) return res;

	res = i2c_write(&reg, 1);
    if (res < 0) return res;

    return i2c_send(addr, data, length);
}
