#include <stdio.h>

#include <ec/i2c.h>

uint8_t smbus_read(uint8_t address, uint8_t command, uint16_t * data) {
    if (i2c_write(address, &command, 1)) return 1;
    return i2c_read(address, (uint8_t *)data, 2);
}

uint8_t smbus_write(uint8_t address, uint8_t command, uint16_t data) {
    if (i2c_write(address, &command, 1)) return 1;
    return i2c_write(address, &data, 2);
}

uint8_t battery_charger_disable(void) {
    uint8_t err = 0;

    // Disable charge current
    err = smbus_write(0x09, 0x14, 0);
    if (err) return err;

    // Disable charge voltage
    err = smbus_write(0x09, 0x15, 0);
    if (err) return err;

    return 0;
}

uint8_t battery_charger_enable(void) {
    uint8_t err = 0;

    err = battery_charger_disable();
    if (err) return err;

    // Set charge current to ~1.5 A
    err = smbus_write(0x09, 0x14, 0x0600);
    if (err) return err;

    // Set charge voltage to ~13 V
    err = smbus_write(0x09, 0x15, 0x3300);
    if (err) return err;

    return 0;
}

void battery_debug(void) {
    uint16_t data = 0;
    uint8_t err = 0;

    #define command(N, A, V) { \
        printf(#N ": "); \
        err = smbus_read(A, V, &data); \
        if (err) { \
            printf("ERROR %02X\n", err); \
        } else { \
            printf("%04X\n", data); \
        } \
    }

    printf("Battery:\n");
    command(Temperature, 0x0B, 0x08);
    command(Voltage, 0x0B, 0x09);
    command(Current, 0x0B, 0x0A);
    command(Charge, 0x0B, 0x0D);

    printf("Charger:\n");
    command(ChargeOption0, 0x09, 0x12);
    command(ChargeOption1, 0x09, 0x3B);
    command(ChargeOption2, 0x09, 0x38);
    command(ChargeOption3, 0x09, 0x37);
    command(ChargeCurrent, 0x09, 0x14);
    command(ChargeVoltage, 0x09, 0x15);
    command(DishargeCurrent, 0x09, 0x39);
    command(InputCurrent, 0x09, 0x3F);
    command(ProchotOption0, 0x09, 0x3C);
    command(ProchotOption1, 0x09, 0x3D);
    command(ProchotStatus, 0x09, 0x3A);

    #undef command
}
