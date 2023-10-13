// SPDX-License-Identifier: GPL-3.0-only

#ifndef _ARCH_I2C_SLAVE_H
#define _ARCH_I2C_SLAVE_H

void i2c_slave_init(
    uint8_t address,
    void (*new_cb)(),
    void (*recv_cb)(uint8_t),
    uint8_t (*send_cb)()
);
void i2c_slave_stop(void);

#endif // _ARCH_I2C_SLAVE_H
