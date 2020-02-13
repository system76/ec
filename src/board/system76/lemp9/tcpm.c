#include <board/gpio.h>
#include <board/tcpm.h>
#include <ec/i2c.h>

void tcpm_init(void) {
    // Set up for i2c usage
    i2c_reset(&I2C_TCPM, true);

    // Enable connection to TCPC
    *(EC_SMD_EN_N.control) = GPIO_OUT;
}

int tcpm_read(uint8_t address, uint8_t command, uint16_t * data) {
    return i2c_get(&I2C_TCPM, address, command, (uint8_t *)data, 2);
}

int tcpm_write(uint8_t address, uint8_t command, uint16_t data) {
    return i2c_set(&I2C_TCPM, address, command, (uint8_t *)&data, 2);
}

void tcpm_event(void) {
    //TODO
}
