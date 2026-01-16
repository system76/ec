// SPDX-License-Identifier: GPL-3.0-only

#include <app/pnp.h>
#include <common/macro.h>
#include <common/debug.h>
#include <soc/espi.h>
#include <stdint.h>

volatile uint8_t __xdata __at(0x1200) IHIOA;
volatile uint8_t __xdata __at(0x1201) IHD;
volatile uint8_t __xdata __at(0x1204) IBMAE;
volatile uint8_t __xdata __at(0x1205) IBCTL;

uint8_t ec2i_read(uint8_t port) {
    uint8_t ihd;

    while (IBCTL & (BIT(2) | BIT(1))) {}
    IHIOA = port;
    IBMAE = BIT(0);
    IBCTL = BIT(1);
    IBCTL |= BIT(0);
    while (IBCTL & BIT(1)) {}
    ihd = IHD;
    IBMAE = 0;
    IBCTL = 0;
    return ihd;
}

void ec2i_write(uint8_t port, uint8_t data) {
    while (IBCTL & (BIT(2) | BIT(1))) {}
    IHIOA = port;
    IHD = data;
    IBMAE = BIT(0);
    IBCTL = BIT(0);
    while (IBCTL & BIT(2)) {}
    IBMAE = 0;
    IBCTL = 0;
}

uint8_t pnp_read(uint8_t reg) {
    ec2i_write(0x2E, reg);
    return ec2i_read(0x2F);
}

void pnp_write(uint8_t reg, uint8_t data) {
    ec2i_write(0x2E, reg);
    ec2i_write(0x2F, data);
}

void pnp_enable(void) {
    DEBUG("Enable PNP devices\n");

    // Enable PMC1
    pnp_write(0x07, 0x11);
    pnp_write(0x30, 0x01);

    // Enable KBC keyboard
    pnp_write(0x07, 0x06);
#if CONFIG_BUS_ESPI
    // When using eSPI, IRQ must be inverted and edge-triggered
    pnp_write(0x71, 0x02);
#endif // CONFIG_BUS_ESPI
    pnp_write(0x30, 0x01);

    // Enable KBC mouse
    pnp_write(0x07, 0x05);
#if CONFIG_BUS_ESPI
    // When using eSPI, IRQ must be inverted and edge-triggered
    pnp_write(0x71, 0x02);
#endif // CONFIG_BUS_ESPI
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
