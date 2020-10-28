// SPDX-License-Identifier: GPL-3.0-only

#include <stdint.h>

#include <common/debug.h>
#include <ec/espi.h>

volatile uint8_t __xdata __at(0x1200) IHIOA;
volatile uint8_t __xdata __at(0x1201) IHD;
volatile uint8_t __xdata __at(0x1204) IBMAE;
volatile uint8_t __xdata __at(0x1205) IBCTL;
void e2ci_write(uint8_t port, uint8_t data) {
    while (IBCTL & ((1 << 2) | (1 << 1))) {}
    IHIOA = port;
    IHD = data;
    IBMAE = 1;
    IBCTL = 1;
    while (IBCTL & (1 << 2)) {}
    IBMAE = 0;
    IBCTL = 0;
}

void pnp_write(uint8_t reg, uint8_t data) {
    e2ci_write(0x2E, reg);
    e2ci_write(0x2F, data);
}

void pnp_enable() {
    DEBUG("Enable PNP devices\n");

    // Enable PMC1
    pnp_write(0x07, 0x11);
    pnp_write(0x30, 0x01);

    // Enable KBC keyboard
    pnp_write(0x07, 0x06);
#if EC_ESPI
    // When using eSPI, IRQ must be inverted and edge-triggered
    pnp_write(0x71, 0x02);
#endif // EC_ESPI
    pnp_write(0x30, 0x01);

    // Enable KBC mouse
    pnp_write(0x07, 0x05);
#if EC_ESPI
    // When using eSPI, IRQ must be inverted and edge-triggered
    pnp_write(0x71, 0x02);
#endif // EC_ESPI
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
