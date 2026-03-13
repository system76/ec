// SPDX-License-Identifier: GPL-3.0-only

#include <app/pnp.h>
#include <common/macro.h>
#include <common/debug.h>
#include <ec/espi.h>
#include <stdint.h>

volatile uint8_t __xdata __at(0x1200) IHIOA;
volatile uint8_t __xdata __at(0x1201) IHD;
volatile uint8_t __xdata __at(0x1204) IBMAE;
volatile uint8_t __xdata __at(0x1205) IBCTL;

// PNP host index register map
enum HostIdx {
    // Logical Device Number
    HOST_IDX_LDN = 0x07,
    // Chip ID Byte 1
    HOST_IDX_CHIPID1 = 0x20,
    // Chip ID Byte 2
    HOST_IDX_CHIPID2 = 0x21,
    // Chip Version
    HOST_IDX_CHIPVER = 0x22,
    // Super I/O Control
    HOST_IDX_SIOCTRL = 0x23,
    // Super I/O IRQ Configuration
    HOST_IDX_SIOIRQ = 0x25,
    // Super I/O General Purpose
    HOST_IDX_SIOGP = 0x26,
    // Super I/O Power Mode
    HOST_IDX_SIOPWR = 0x2D,
    // Depth 2 I/O Address
    HOST_IDX_D2ADR = 0x2E,
    // Depth 2 I/O Data
    HOST_IDX_D2DAT = 0x2F,
    // Logical Device Activate Register
    HOST_IDX_LDA = 0x30,
    // I/O Port Base Address Bits [15:8] for Descriptor 0
    HOST_IDX_IOBAD0_HI = 0x60,
    // I/O Port Base Address Bits [7:0] for Descriptor 0
    HOST_IDX_IOBAD0_LO = 0x61,
    // I/O Port Base Address Bits [15:8] for Descriptor 1
    HOST_IDX_IOBAD1_HI = 0x62,
    // I/O Port Base Address Bits [7:0] for Descriptor 1
    HOST_IDX_IOBAD1_LO = 0x63,
    // Interrupt Request Number and Wake-Up on IRQ Enabled
    HOST_IDX_IRQNUMX = 0x70,
    // Interrupt Request Type Select
    HOST_IDX_IRQTP = 0x71,
    // DMA Channel Select 0
    HOST_IDX_DMAS0 = 0x74,
    // DMA Channel Select 1
    HOST_IDX_DMAS1 = 0x75,
    // 0xF0-0xF9:  Device-specific configuration data
    HOST_IDX_DSD1 = 0xF0,
    HOST_IDX_DSD2 = 0xF1,
    HOST_IDX_DSD3 = 0xF2,
    HOST_IDX_DSD4 = 0xF3,
    HOST_IDX_DSD5 = 0xF4,
    HOST_IDX_DSD6 = 0xF5,
    HOST_IDX_DSD7 = 0xF6,
    HOST_IDX_DSD8 = 0xF7,
    HOST_IDX_DSD9 = 0xF8,
    HOST_IDX_DSD10 = 0xF9,
};

enum LogicalDeviceNumber {
    LDN_UART1 = 0x01,
    LDN_UART2 = 0x02,
    LDN_SWUC = 0x04,
    LDN_KBC_MOUSE = 0x05,
    LDN_KBC_KEYBOARD = 0x06,
    LDN_CIR = 0x0A,
    LDN_SMFI = 0x0F,
    LDN_RTCT = 0x10,
    LDN_PMC1 = 0x11,
    LDN_PMC2 = 0x12,
    LDN_SSPI = 0x13,
    LDN_PECI = 0x14,
    LDN_PMC3 = 0x17,
    LDN_PMC4 = 0x18,
    LDN_PMC5 = 0x19,
};

uint8_t ec2i_read(uint8_t port) {
    uint8_t ihd;

    while (IBCTL & (BIT(2) | BIT(1))) {}
    IHIOA = port;
    IBMAE |= BIT(0);
    IBCTL |= BIT(1);
    IBCTL |= BIT(0);
    while (IBCTL & BIT(1)) {}
    ihd = IHD;
    IBMAE &= ~BIT(0);
    IBCTL &= ~BIT(0);
    return ihd;
}

void ec2i_write(uint8_t port, uint8_t data) {
    while (IBCTL & (BIT(2) | BIT(1))) {}
    IHIOA = port;
    IHD = data;
    IBMAE |= BIT(0);
    IBCTL |= BIT(0);
    while (IBCTL & BIT(2)) {}
    IBMAE &= ~BIT(0);
    IBCTL &= ~BIT(0);
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
    pnp_write(HOST_IDX_LDN, LDN_PMC1);
    pnp_write(HOST_IDX_LDA, 0x01);

    // Enable KBC keyboard
    pnp_write(HOST_IDX_LDN, LDN_KBC_KEYBOARD);
#if CONFIG_BUS_ESPI
    // When using eSPI, IRQ must be inverted and edge-triggered
    pnp_write(HOST_IDX_IRQTP, 0x02);
#endif // CONFIG_BUS_ESPI
    pnp_write(HOST_IDX_LDA, 0x01);

    // Enable KBC mouse
    pnp_write(HOST_IDX_LDN, LDN_KBC_MOUSE);
#if CONFIG_BUS_ESPI
    // When using eSPI, IRQ must be inverted and edge-triggered
    pnp_write(HOST_IDX_IRQTP, 0x02);
#endif // CONFIG_BUS_ESPI
    pnp_write(HOST_IDX_LDA, 0x01);

    // Enable SMFI
    pnp_write(HOST_IDX_LDN, LDN_SMFI);
    // Enable H2RAM through LPC IO cycle
    pnp_write(HOST_IDX_DSD6, 0x00);
    pnp_write(HOST_IDX_DSD7, 0x01);
    pnp_write(HOST_IDX_LDA, 0x01);

    // Enable SWUC
    pnp_write(HOST_IDX_LDN, LDN_SWUC);
    pnp_write(HOST_IDX_LDA, 0x01);
}
