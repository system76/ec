#include <stdio.h>

#include <common/i2c.h>

int smbus_read(uint8_t address, uint8_t command, uint16_t * data) {
    return i2c_get(address, command, (uint8_t *)data, 2);
}

int smbus_write(uint8_t address, uint8_t command, uint16_t data) {
    return i2c_set(address, command, (uint8_t *)&data, 2);
}

void battery_debug(void) {
    uint16_t data = 0;
    int res = 0;

    #define command(N, A, V) { \
        printf(#N ": "); \
        res = smbus_read(A, V, &data); \
        if (res < 0) { \
            printf("ERROR %04X\n", -res); \
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
