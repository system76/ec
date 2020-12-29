// SPDX-License-Identifier: GPL-3.0-only

#ifndef _EC_GPIO_H
#define _EC_GPIO_H

#include <stdbool.h>
#include <stdint.h>

struct Gpio {
    volatile uint8_t __xdata * data;
    volatile uint8_t __xdata * mirror;
    volatile uint8_t __xdata * control;
    uint8_t value;
};

#define GPIO(BLOCK, NUMBER) { \
    .data = &GPDR ## BLOCK, \
    .mirror = &GPDMR ## BLOCK, \
    .control = &GPCR ## BLOCK ## NUMBER, \
    .value = (1 << NUMBER), \
}

bool gpio_get(struct Gpio * gpio);
void gpio_set(struct Gpio * gpio, bool value);

volatile uint8_t __xdata __at(0x1600) GCR;
volatile uint8_t __xdata __at(0x16F0) GCR1;
volatile uint8_t __xdata __at(0x16F1) GCR2;
volatile uint8_t __xdata __at(0x16F2) GCR3;
volatile uint8_t __xdata __at(0x16F3) GCR4;
volatile uint8_t __xdata __at(0x16F4) GCR5;
volatile uint8_t __xdata __at(0x16F5) GCR6;
volatile uint8_t __xdata __at(0x16F6) GCR7;
volatile uint8_t __xdata __at(0x16F7) GCR8;
volatile uint8_t __xdata __at(0x16F8) GCR9;
volatile uint8_t __xdata __at(0x16F9) GCR10;
volatile uint8_t __xdata __at(0x16FA) GCR11;
volatile uint8_t __xdata __at(0x16FB) GCR12;
volatile uint8_t __xdata __at(0x16FC) GCR13;
volatile uint8_t __xdata __at(0x16FD) GCR14;
volatile uint8_t __xdata __at(0x16FE) GCR15;
volatile uint8_t __xdata __at(0x16E0) GCR16;
volatile uint8_t __xdata __at(0x16E1) GCR17;
volatile uint8_t __xdata __at(0x16E2) GCR18;
volatile uint8_t __xdata __at(0x16E4) GCR19;
volatile uint8_t __xdata __at(0x16E5) GCR20;
volatile uint8_t __xdata __at(0x16E6) GCR21;

volatile uint8_t __xdata __at(0x1601) GPDRA;
volatile uint8_t __xdata __at(0x1602) GPDRB;
volatile uint8_t __xdata __at(0x1603) GPDRC;
volatile uint8_t __xdata __at(0x1604) GPDRD;
volatile uint8_t __xdata __at(0x1605) GPDRE;
volatile uint8_t __xdata __at(0x1606) GPDRF;
volatile uint8_t __xdata __at(0x1607) GPDRG;
volatile uint8_t __xdata __at(0x1608) GPDRH;
volatile uint8_t __xdata __at(0x1609) GPDRI;
volatile uint8_t __xdata __at(0x160A) GPDRJ;
volatile uint8_t __xdata __at(0x160D) GPDRM;

volatile uint8_t __xdata __at(0x1661) GPDMRA;
volatile uint8_t __xdata __at(0x1662) GPDMRB;
volatile uint8_t __xdata __at(0x1663) GPDMRC;
volatile uint8_t __xdata __at(0x1664) GPDMRD;
volatile uint8_t __xdata __at(0x1665) GPDMRE;
volatile uint8_t __xdata __at(0x1666) GPDMRF;
volatile uint8_t __xdata __at(0x1667) GPDMRG;
volatile uint8_t __xdata __at(0x1668) GPDMRH;
volatile uint8_t __xdata __at(0x1669) GPDMRI;
volatile uint8_t __xdata __at(0x166A) GPDMRJ;
volatile uint8_t __xdata __at(0x166D) GPDMRM;

volatile uint8_t __xdata __at(0x1671) GPOTA;
volatile uint8_t __xdata __at(0x1672) GPOTB;
volatile uint8_t __xdata __at(0x1673) GPOTC;
volatile uint8_t __xdata __at(0x1674) GPOTD;
volatile uint8_t __xdata __at(0x1675) GPOTE;
volatile uint8_t __xdata __at(0x1676) GPOTF;
volatile uint8_t __xdata __at(0x1677) GPOTG;
volatile uint8_t __xdata __at(0x1678) GPOTH;
volatile uint8_t __xdata __at(0x1679) GPOTI;
volatile uint8_t __xdata __at(0x167A) GPOTJ;
// GPOTM does not exist

volatile uint8_t __xdata __at(0x1610) GPCRA0;
volatile uint8_t __xdata __at(0x1611) GPCRA1;
volatile uint8_t __xdata __at(0x1612) GPCRA2;
volatile uint8_t __xdata __at(0x1613) GPCRA3;
volatile uint8_t __xdata __at(0x1614) GPCRA4;
volatile uint8_t __xdata __at(0x1615) GPCRA5;
volatile uint8_t __xdata __at(0x1616) GPCRA6;
volatile uint8_t __xdata __at(0x1617) GPCRA7;

volatile uint8_t __xdata __at(0x1618) GPCRB0;
volatile uint8_t __xdata __at(0x1619) GPCRB1;
volatile uint8_t __xdata __at(0x161A) GPCRB2;
volatile uint8_t __xdata __at(0x161B) GPCRB3;
volatile uint8_t __xdata __at(0x161C) GPCRB4;
volatile uint8_t __xdata __at(0x161D) GPCRB5;
volatile uint8_t __xdata __at(0x161E) GPCRB6;
volatile uint8_t __xdata __at(0x161F) GPCRB7;

volatile uint8_t __xdata __at(0x1620) GPCRC0;
volatile uint8_t __xdata __at(0x1621) GPCRC1;
volatile uint8_t __xdata __at(0x1622) GPCRC2;
volatile uint8_t __xdata __at(0x1623) GPCRC3;
volatile uint8_t __xdata __at(0x1624) GPCRC4;
volatile uint8_t __xdata __at(0x1625) GPCRC5;
volatile uint8_t __xdata __at(0x1626) GPCRC6;
volatile uint8_t __xdata __at(0x1627) GPCRC7;

volatile uint8_t __xdata __at(0x1628) GPCRD0;
volatile uint8_t __xdata __at(0x1629) GPCRD1;
volatile uint8_t __xdata __at(0x162A) GPCRD2;
volatile uint8_t __xdata __at(0x162B) GPCRD3;
volatile uint8_t __xdata __at(0x162C) GPCRD4;
volatile uint8_t __xdata __at(0x162D) GPCRD5;
volatile uint8_t __xdata __at(0x162E) GPCRD6;
volatile uint8_t __xdata __at(0x162F) GPCRD7;

volatile uint8_t __xdata __at(0x1630) GPCRE0;
volatile uint8_t __xdata __at(0x1631) GPCRE1;
volatile uint8_t __xdata __at(0x1632) GPCRE2;
volatile uint8_t __xdata __at(0x1633) GPCRE3;
volatile uint8_t __xdata __at(0x1634) GPCRE4;
volatile uint8_t __xdata __at(0x1635) GPCRE5;
volatile uint8_t __xdata __at(0x1636) GPCRE6;
volatile uint8_t __xdata __at(0x1637) GPCRE7;

volatile uint8_t __xdata __at(0x1638) GPCRF0;
volatile uint8_t __xdata __at(0x1639) GPCRF1;
volatile uint8_t __xdata __at(0x163A) GPCRF2;
volatile uint8_t __xdata __at(0x163B) GPCRF3;
volatile uint8_t __xdata __at(0x163C) GPCRF4;
volatile uint8_t __xdata __at(0x163D) GPCRF5;
volatile uint8_t __xdata __at(0x163E) GPCRF6;
volatile uint8_t __xdata __at(0x163F) GPCRF7;

volatile uint8_t __xdata __at(0x1640) GPCRG0;
volatile uint8_t __xdata __at(0x1641) GPCRG1;
volatile uint8_t __xdata __at(0x1642) GPCRG2;
volatile uint8_t __xdata __at(0x1643) GPCRG3;
volatile uint8_t __xdata __at(0x1644) GPCRG4;
volatile uint8_t __xdata __at(0x1645) GPCRG5;
volatile uint8_t __xdata __at(0x1646) GPCRG6;
volatile uint8_t __xdata __at(0x1647) GPCRG7;

volatile uint8_t __xdata __at(0x1648) GPCRH0;
volatile uint8_t __xdata __at(0x1649) GPCRH1;
volatile uint8_t __xdata __at(0x164A) GPCRH2;
volatile uint8_t __xdata __at(0x164B) GPCRH3;
volatile uint8_t __xdata __at(0x164C) GPCRH4;
volatile uint8_t __xdata __at(0x164D) GPCRH5;
volatile uint8_t __xdata __at(0x164E) GPCRH6;
volatile uint8_t __xdata __at(0x164F) GPCRH7;

volatile uint8_t __xdata __at(0x1650) GPCRI0;
volatile uint8_t __xdata __at(0x1651) GPCRI1;
volatile uint8_t __xdata __at(0x1652) GPCRI2;
volatile uint8_t __xdata __at(0x1653) GPCRI3;
volatile uint8_t __xdata __at(0x1654) GPCRI4;
volatile uint8_t __xdata __at(0x1655) GPCRI5;
volatile uint8_t __xdata __at(0x1656) GPCRI6;
volatile uint8_t __xdata __at(0x1657) GPCRI7;

volatile uint8_t __xdata __at(0x1658) GPCRJ0;
volatile uint8_t __xdata __at(0x1659) GPCRJ1;
volatile uint8_t __xdata __at(0x165A) GPCRJ2;
volatile uint8_t __xdata __at(0x165B) GPCRJ3;
volatile uint8_t __xdata __at(0x165C) GPCRJ4;
volatile uint8_t __xdata __at(0x165D) GPCRJ5;
volatile uint8_t __xdata __at(0x165E) GPCRJ6;
volatile uint8_t __xdata __at(0x165F) GPCRJ7;

volatile uint8_t __xdata __at(0x16A0) GPCRM0;
volatile uint8_t __xdata __at(0x16A1) GPCRM1;
volatile uint8_t __xdata __at(0x16A2) GPCRM2;
volatile uint8_t __xdata __at(0x16A3) GPCRM3;
volatile uint8_t __xdata __at(0x16A4) GPCRM4;
volatile uint8_t __xdata __at(0x16A5) GPCRM5;
volatile uint8_t __xdata __at(0x16A6) GPCRM6;

#endif // _EC_GPIO_H
