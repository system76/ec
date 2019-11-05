#include <stdio.h>

#include <board/smbus.h>

uint8_t smbus_read(uint8_t address, uint8_t command, uint16_t * data) {
    // Wait for last command
    while (HOSTAA & 1) {}

    // Clear result
    HOSTAA = HOSTAA;

    // Clock to 400 KHz
    SCLKTSA = 3;

    // Enable host interface
    HOCTL2A = 1 << 0;

    // Read value from address
    TRASLAA = (address << 1) | (1 << 0);
    HOCMDA = command;

    // Start read word command
    HOCTLA = (1 << 6) | (0b011 << 2);

    // Wait for command to start
    while (!(HOSTAA & 1)) {}

    // Wait for command to finish
    while (HOSTAA & 1) {}

    // Read and clear status
    uint8_t status = HOSTAA;
    HOSTAA = status;

    // Disable host interface
    HOCTL2A = 0;

    // If there were no errors, set value and return 0
    uint8_t error = (1 << 6) | (1 << 5) | (1 << 4) | (1 << 3) | (1 << 2);
    if (!(status & error)) {
        *data = ((uint16_t)D0REGA) |
                ((uint16_t)D1REGA << 8);
        return 0;
    } else {
        //TODO: custom error type or flags for errors
        return (status & error);
    }
}

uint8_t smbus_write(uint8_t address, uint8_t command, uint16_t data) {
    // Wait for last command
    while (HOSTAA & 1) {}

    // Clear result
    HOSTAA = HOSTAA;

    // Clock to 400 KHz
    SCLKTSA = 3;

    // Enable host interface
    HOCTL2A = 1 << 0;

    // Write value to address
    TRASLAA = (address << 1);
    HOCMDA = command;

    D0REGA = (uint8_t)data;
    D1REGA = (uint8_t)(data >> 8);

    // Start read word command
    HOCTLA = (1 << 6) | (0b011 << 2);

    // Wait for command to start
    while (!(HOSTAA & 1)) {}

    // Wait for command to finish
    while (HOSTAA & 1) {}

    // Read and clear status
    uint8_t status = HOSTAA;
    HOSTAA = status;

    // Disable host interface
    HOCTL2A = 0;

    // If there were no errors, set value and return 0
    uint8_t error = (1 << 6) | (1 << 5) | (1 << 4) | (1 << 3) | (1 << 2);
    //TODO: custom error type or flags for errors
    return (status & error);
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
