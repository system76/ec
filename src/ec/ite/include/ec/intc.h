// SPDX-License-Identifier: GPL-3.0-only
// SPDX-FileCopyrightText: 2022 System76, Inc.

// Interrupt Controller (INTC)

// ISR: Interrupt Status Register
// IER: Interrupt Enable Register
// IELMR: Interrupt Edge/Level-Triggered Mode Register
// IPOLR: Interrupt Polarity Register

#ifndef _EC_INTC_H
#define _EC_INTC_H

#include <stdint.h>

volatile uint8_t __xdata __at(0x1100) ISR0;
volatile uint8_t __xdata __at(0x1104) IER0;
volatile uint8_t __xdata __at(0x1108) IELMR0;
volatile uint8_t __xdata __at(0x110C) IPOLR0;

volatile uint8_t __xdata __at(0x1101) ISR1;
volatile uint8_t __xdata __at(0x1105) IER1;
volatile uint8_t __xdata __at(0x1109) IELMR1;
volatile uint8_t __xdata __at(0x110D) IPOLR1;

volatile uint8_t __xdata __at(0x1102) ISR2;
volatile uint8_t __xdata __at(0x1106) IER2;
volatile uint8_t __xdata __at(0x110A) IELMR2;
volatile uint8_t __xdata __at(0x110E) IPOLR2;

volatile uint8_t __xdata __at(0x1103) ISR3;
volatile uint8_t __xdata __at(0x1107) IER3;
volatile uint8_t __xdata __at(0x110B) IELMR3;
volatile uint8_t __xdata __at(0x110F) IPOLR3;

volatile uint8_t __xdata __at(0x1114) ISR4;
volatile uint8_t __xdata __at(0x1115) IER4;
volatile uint8_t __xdata __at(0x1116) IELMR4;
volatile uint8_t __xdata __at(0x1117) IPOLR4;

volatile uint8_t __xdata __at(0x1118) ISR5;
volatile uint8_t __xdata __at(0x1119) IER5;
volatile uint8_t __xdata __at(0x111A) IELMR5;
volatile uint8_t __xdata __at(0x111B) IPOLR5;

volatile uint8_t __xdata __at(0x111C) ISR6;
volatile uint8_t __xdata __at(0x111D) IER6;
volatile uint8_t __xdata __at(0x111E) IELMR6;
volatile uint8_t __xdata __at(0x111F) IPOLR6;

volatile uint8_t __xdata __at(0x1120) ISR7;
volatile uint8_t __xdata __at(0x1121) IER7;
volatile uint8_t __xdata __at(0x1122) IELMR7;
volatile uint8_t __xdata __at(0x1123) IPOLR7;

volatile uint8_t __xdata __at(0x1124) ISR8;
volatile uint8_t __xdata __at(0x1125) IER8;
volatile uint8_t __xdata __at(0x1126) IELMR8;
volatile uint8_t __xdata __at(0x1127) IPOLR8;

volatile uint8_t __xdata __at(0x1128) ISR9;
volatile uint8_t __xdata __at(0x1129) IER9;
volatile uint8_t __xdata __at(0x112A) IELMR9;
volatile uint8_t __xdata __at(0x112B) IPOLR9;

volatile uint8_t __xdata __at(0x112C) ISR10;
volatile uint8_t __xdata __at(0x112D) IER10;
volatile uint8_t __xdata __at(0x112E) IELMR10;
volatile uint8_t __xdata __at(0x112F) IPOLR10;

volatile uint8_t __xdata __at(0x1130) ISR11;
volatile uint8_t __xdata __at(0x1131) IER11;
volatile uint8_t __xdata __at(0x1132) IELMR11;
volatile uint8_t __xdata __at(0x1133) IPOLR11;

volatile uint8_t __xdata __at(0x1134) ISR12;
volatile uint8_t __xdata __at(0x1135) IER12;
volatile uint8_t __xdata __at(0x1136) IELMR12;
volatile uint8_t __xdata __at(0x1137) IPOLR12;

volatile uint8_t __xdata __at(0x1138) ISR13;
volatile uint8_t __xdata __at(0x1139) IER13;
volatile uint8_t __xdata __at(0x113A) IELMR13;
volatile uint8_t __xdata __at(0x113B) IPOLR13;

volatile uint8_t __xdata __at(0x113C) ISR14;
volatile uint8_t __xdata __at(0x113D) IER14;
volatile uint8_t __xdata __at(0x113E) IELMR14;
volatile uint8_t __xdata __at(0x113F) IPOLR14;

volatile uint8_t __xdata __at(0x1140) ISR15;
volatile uint8_t __xdata __at(0x1141) IER15;
volatile uint8_t __xdata __at(0x1142) IELMR15;
volatile uint8_t __xdata __at(0x1143) IPOLR15;

volatile uint8_t __xdata __at(0x1144) ISR16;
volatile uint8_t __xdata __at(0x1145) IER16;
volatile uint8_t __xdata __at(0x1146) IELMR16;
volatile uint8_t __xdata __at(0x1147) IPOLR16;

volatile uint8_t __xdata __at(0x1148) ISR17;
volatile uint8_t __xdata __at(0x1149) IER17;
volatile uint8_t __xdata __at(0x114A) IELMR17;
volatile uint8_t __xdata __at(0x114B) IPOLR17;

volatile uint8_t __xdata __at(0x114C) ISR18;
volatile uint8_t __xdata __at(0x114D) IER18;
volatile uint8_t __xdata __at(0x114E) IELMR18;
volatile uint8_t __xdata __at(0x114F) IPOLR18;

#if CONFIG_EC_ITE_IT5570E || CONFIG_EC_ITE_IT5571E
volatile uint8_t __xdata __at(0x1150) ISR19;
volatile uint8_t __xdata __at(0x1151) IER19;
volatile uint8_t __xdata __at(0x1152) IELMR19;
volatile uint8_t __xdata __at(0x1153) IPOLR19;

volatile uint8_t __xdata __at(0x1154) ISR20;
volatile uint8_t __xdata __at(0x1155) IER20;
volatile uint8_t __xdata __at(0x1156) IELMR20;
volatile uint8_t __xdata __at(0x1157) IPOLR20;

volatile uint8_t __xdata __at(0x1158) ISR21;
volatile uint8_t __xdata __at(0x1159) IER21;
volatile uint8_t __xdata __at(0x115A) IELMR21;
volatile uint8_t __xdata __at(0x115B) IPOLR21;
#endif

// Interrupt Vector Register
volatile uint8_t __xdata __at(0x1110) IVCT;

/**
 * Return the highest priority pending IRQ.
 */
static inline uint8_t intc_get_irq(void) {
    return IVCT - 0x10;
}

void intc_enable(uint8_t nr);
void intc_disable(uint8_t nr);
void intc_clear(uint8_t nr);

#endif // _EC_INTC_H
