#ifndef _EC_GPIO_H
#define _EC_GPIO_H

#include <stdbool.h>
#include <stdint.h>

struct Gpio {
    __xdata volatile uint8_t * data;
    __xdata volatile uint8_t * mirror;
    __xdata volatile uint8_t * control;
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

__xdata volatile uint8_t __at(0x1600) GCR;

__xdata volatile uint8_t __at(0x1601) GPDRA;
__xdata volatile uint8_t __at(0x1602) GPDRB;
__xdata volatile uint8_t __at(0x1603) GPDRC;
__xdata volatile uint8_t __at(0x1604) GPDRD;
__xdata volatile uint8_t __at(0x1605) GPDRE;
__xdata volatile uint8_t __at(0x1606) GPDRF;
__xdata volatile uint8_t __at(0x1607) GPDRG;
__xdata volatile uint8_t __at(0x1608) GPDRH;
__xdata volatile uint8_t __at(0x1609) GPDRI;
__xdata volatile uint8_t __at(0x160A) GPDRJ;
__xdata volatile uint8_t __at(0x160D) GPDRM;

__xdata volatile uint8_t __at(0x1661) GPDMRA;
__xdata volatile uint8_t __at(0x1662) GPDMRB;
__xdata volatile uint8_t __at(0x1663) GPDMRC;
__xdata volatile uint8_t __at(0x1664) GPDMRD;
__xdata volatile uint8_t __at(0x1665) GPDMRE;
__xdata volatile uint8_t __at(0x1666) GPDMRF;
__xdata volatile uint8_t __at(0x1667) GPDMRG;
__xdata volatile uint8_t __at(0x1668) GPDMRH;
__xdata volatile uint8_t __at(0x1669) GPDMRI;
__xdata volatile uint8_t __at(0x166A) GPDMRJ;
__xdata volatile uint8_t __at(0x166D) GPDMRM;

__xdata volatile uint8_t __at(0x1671) GPOTA;
__xdata volatile uint8_t __at(0x1672) GPOTB;
__xdata volatile uint8_t __at(0x1673) GPOTC;
__xdata volatile uint8_t __at(0x1674) GPOTD;
__xdata volatile uint8_t __at(0x1675) GPOTE;
__xdata volatile uint8_t __at(0x1676) GPOTF;
__xdata volatile uint8_t __at(0x1677) GPOTG;
__xdata volatile uint8_t __at(0x1678) GPOTH;
__xdata volatile uint8_t __at(0x1679) GPOTI;
__xdata volatile uint8_t __at(0x167A) GPOTJ;
// GPOTM does not exist

__xdata volatile uint8_t __at(0x1610) GPCRA0;
__xdata volatile uint8_t __at(0x1611) GPCRA1;
__xdata volatile uint8_t __at(0x1612) GPCRA2;
__xdata volatile uint8_t __at(0x1613) GPCRA3;
__xdata volatile uint8_t __at(0x1614) GPCRA4;
__xdata volatile uint8_t __at(0x1615) GPCRA5;
__xdata volatile uint8_t __at(0x1616) GPCRA6;
__xdata volatile uint8_t __at(0x1617) GPCRA7;

__xdata volatile uint8_t __at(0x1618) GPCRB0;
__xdata volatile uint8_t __at(0x1619) GPCRB1;
__xdata volatile uint8_t __at(0x161A) GPCRB2;
__xdata volatile uint8_t __at(0x161B) GPCRB3;
__xdata volatile uint8_t __at(0x161C) GPCRB4;
__xdata volatile uint8_t __at(0x161D) GPCRB5;
__xdata volatile uint8_t __at(0x161E) GPCRB6;
__xdata volatile uint8_t __at(0x161F) GPCRB7;

__xdata volatile uint8_t __at(0x1620) GPCRC0;
__xdata volatile uint8_t __at(0x1621) GPCRC1;
__xdata volatile uint8_t __at(0x1622) GPCRC2;
__xdata volatile uint8_t __at(0x1623) GPCRC3;
__xdata volatile uint8_t __at(0x1624) GPCRC4;
__xdata volatile uint8_t __at(0x1625) GPCRC5;
__xdata volatile uint8_t __at(0x1626) GPCRC6;
__xdata volatile uint8_t __at(0x1627) GPCRC7;

__xdata volatile uint8_t __at(0x1628) GPCRD0;
__xdata volatile uint8_t __at(0x1629) GPCRD1;
__xdata volatile uint8_t __at(0x162A) GPCRD2;
__xdata volatile uint8_t __at(0x162B) GPCRD3;
__xdata volatile uint8_t __at(0x162C) GPCRD4;
__xdata volatile uint8_t __at(0x162D) GPCRD5;
__xdata volatile uint8_t __at(0x162E) GPCRD6;
__xdata volatile uint8_t __at(0x162F) GPCRD7;

__xdata volatile uint8_t __at(0x1630) GPCRE0;
__xdata volatile uint8_t __at(0x1631) GPCRE1;
__xdata volatile uint8_t __at(0x1632) GPCRE2;
__xdata volatile uint8_t __at(0x1633) GPCRE3;
__xdata volatile uint8_t __at(0x1634) GPCRE4;
__xdata volatile uint8_t __at(0x1635) GPCRE5;
__xdata volatile uint8_t __at(0x1636) GPCRE6;
__xdata volatile uint8_t __at(0x1637) GPCRE7;

__xdata volatile uint8_t __at(0x1638) GPCRF0;
__xdata volatile uint8_t __at(0x1639) GPCRF1;
__xdata volatile uint8_t __at(0x163A) GPCRF2;
__xdata volatile uint8_t __at(0x163B) GPCRF3;
__xdata volatile uint8_t __at(0x163C) GPCRF4;
__xdata volatile uint8_t __at(0x163D) GPCRF5;
__xdata volatile uint8_t __at(0x163E) GPCRF6;
__xdata volatile uint8_t __at(0x163F) GPCRF7;

__xdata volatile uint8_t __at(0x1640) GPCRG0;
__xdata volatile uint8_t __at(0x1641) GPCRG1;
__xdata volatile uint8_t __at(0x1642) GPCRG2;
__xdata volatile uint8_t __at(0x1643) GPCRG3;
__xdata volatile uint8_t __at(0x1644) GPCRG4;
__xdata volatile uint8_t __at(0x1645) GPCRG5;
__xdata volatile uint8_t __at(0x1646) GPCRG6;
__xdata volatile uint8_t __at(0x1647) GPCRG7;

__xdata volatile uint8_t __at(0x1648) GPCRH0;
__xdata volatile uint8_t __at(0x1649) GPCRH1;
__xdata volatile uint8_t __at(0x164A) GPCRH2;
__xdata volatile uint8_t __at(0x164B) GPCRH3;
__xdata volatile uint8_t __at(0x164C) GPCRH4;
__xdata volatile uint8_t __at(0x164D) GPCRH5;
__xdata volatile uint8_t __at(0x164E) GPCRH6;
__xdata volatile uint8_t __at(0x164F) GPCRH7;

__xdata volatile uint8_t __at(0x1650) GPCRI0;
__xdata volatile uint8_t __at(0x1651) GPCRI1;
__xdata volatile uint8_t __at(0x1652) GPCRI2;
__xdata volatile uint8_t __at(0x1653) GPCRI3;
__xdata volatile uint8_t __at(0x1654) GPCRI4;
__xdata volatile uint8_t __at(0x1655) GPCRI5;
__xdata volatile uint8_t __at(0x1656) GPCRI6;
__xdata volatile uint8_t __at(0x1657) GPCRI7;

__xdata volatile uint8_t __at(0x1658) GPCRJ0;
__xdata volatile uint8_t __at(0x1659) GPCRJ1;
__xdata volatile uint8_t __at(0x165A) GPCRJ2;
__xdata volatile uint8_t __at(0x165B) GPCRJ3;
__xdata volatile uint8_t __at(0x165C) GPCRJ4;
__xdata volatile uint8_t __at(0x165D) GPCRJ5;
__xdata volatile uint8_t __at(0x165E) GPCRJ6;
__xdata volatile uint8_t __at(0x165F) GPCRJ7;

__xdata volatile uint8_t __at(0x16A0) GPCRM0;
__xdata volatile uint8_t __at(0x16A1) GPCRM1;
__xdata volatile uint8_t __at(0x16A2) GPCRM2;
__xdata volatile uint8_t __at(0x16A3) GPCRM3;
__xdata volatile uint8_t __at(0x16A4) GPCRM4;
__xdata volatile uint8_t __at(0x16A5) GPCRM5;
__xdata volatile uint8_t __at(0x16A6) GPCRM6;

#endif // _EC_GPIO_H
