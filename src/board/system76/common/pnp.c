#include <stdint.h>

#include <common/debug.h>

volatile uint8_t __xdata __at(0x1200) IHIOA;
volatile uint8_t __xdata __at(0x1201) IHD;
volatile uint8_t __xdata __at(0x1204) IBMAE;
volatile uint8_t __xdata __at(0x1205) IBCTL;

uint8_t ec2i_read(uint8_t port) {
    while (IBCTL & ((1 << 2) | (1 << 1))) {}
    IBCTL = 1;
    IBMAE = 1;
    IHIOA = port;
    IBCTL |= (1 << 1);
    while (IBCTL & (1 << 1)) {}
    return IHD;
}

void ec2i_write(uint8_t port, uint8_t data) {
    while (IBCTL & ((1 << 2) | (1 << 1))) {}
    IBCTL = 1;
    IBMAE = 1;
    IHIOA = port;
    IHD = data;
    while (IBCTL & (1 << 2)) {}
}

uint8_t pnp_read(uint8_t reg) {
    ec2i_write(0x2E, reg);
    return ec2i_read(0x2F);
}

void pnp_write(uint8_t reg, uint8_t data) {
    ec2i_write(0x2E, reg);
    ec2i_write(0x2F, data);
}

void pnp_enable() {
    DEBUG("Enable PNP devices\n");

    // Enable PMC1
    pnp_write(0x07, 0x11);
    pnp_write(0x30, 0x01);

    // Enable KBC keyboard
    pnp_write(0x07, 0x06);
    pnp_write(0x30, 0x01);

    // Enable KBC mouse
    pnp_write(0x07, 0x05);
    pnp_write(0x30, 0x01);

    // Enable SMFI
    pnp_write(0x07, 0x0F);
    pnp_write(0xF5, 0x00);
    pnp_write(0xF6, 0x01);
    pnp_write(0x30, 0x01);

    // Enable SWUC
    pnp_write(0x07, 0x04);
    pnp_write(0x30, 0x01);
}
