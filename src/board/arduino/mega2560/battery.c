#include <stdio.h>

#include <board/i2c.h>

uint8_t smbus_read(uint8_t address, uint8_t command, uint16_t * data) {
    return i2c_get(address, command, (uint8_t *)data, 2);
}

void battery_debug(void) {
    uint16_t data = 0;
    uint8_t err = 0;

    #define command(N, A, V) { \
        printf(#N ": "); \
        err = smbus_read(A, V, &data); \
        if (err) { \
            printf("ERROR %02X\n", err); \
            return; \
        } \
        printf("%04X\n", data); \
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
