// SPDX-License-Identifier: GPL-3.0-only

// Based on https://github.com/thegouger/avr-i2c-slave

#include <stdio.h>
#include <avr/io.h>
#include <util/twi.h>
#include <avr/interrupt.h>

#include <board/cpu.h>
#include <arch/i2c_slave.h>
#include <common/macro.h>

static void (*volatile i2c_slave_new_cb)() = NULL;
static void (*volatile i2c_slave_recv_cb)(uint8_t) = NULL;
static uint8_t (*volatile i2c_slave_send_cb)() = NULL;

void i2c_slave_init(
    uint8_t address,
    void (*new_cb)(),
    void (*recv_cb)(uint8_t),
    uint8_t (*send_cb)()
) {
    // ensure correct behavior by stopping before changing callbacks or address
    i2c_slave_stop();

    // clear interrupts
    cli();

    // setup callbacks
    i2c_slave_new_cb = new_cb;
    i2c_slave_recv_cb = recv_cb;
    i2c_slave_send_cb = send_cb;
    // load address into TWI address register
    TWAR = (address << 1);
    // set the TWCR to enable address matching and enable TWI, clear TWINT, enable TWI interrupt
    TWCR = BIT(TWIE) | BIT(TWEA) | BIT(TWINT) | BIT(TWEN);

    // set interrupts
    sei();
}

void i2c_slave_stop(void) {
    // clear interrupts
    cli();

    // clear acknowledge and enable bits
    TWCR &= ~(BIT(TWEA) | BIT(TWEN));
    // clear address
    TWAR = 0;
    // remove callbacks
    i2c_slave_new_cb = NULL;
    i2c_slave_recv_cb = NULL;
    i2c_slave_send_cb = NULL;

    // set interrupts
    sei();
}

ISR(TWI_vect) {
    uint8_t status = TW_STATUS;
    switch (status) {
    case TW_SR_SLA_ACK:
        // master has started a new transaction, call the new callback
        if (i2c_slave_new_cb != NULL) {
            i2c_slave_new_cb();
        }
        TWCR = BIT(TWIE) | BIT(TWINT) | BIT(TWEA) | BIT(TWEN);
        break;
    case TW_SR_DATA_ACK:
        // received data from master, call the receive callback
        if (i2c_slave_send_cb != NULL) {
            i2c_slave_recv_cb(TWDR);
        }
        TWCR = BIT(TWIE) | BIT(TWINT) | BIT(TWEA) | BIT(TWEN);
        break;
    case TW_ST_SLA_ACK:
    case TW_ST_DATA_ACK:
        // master is requesting data, call the send callback
        if (i2c_slave_recv_cb != NULL) {
            TWDR = i2c_slave_send_cb();
        }
        TWCR = BIT(TWIE) | BIT(TWINT) | BIT(TWEA) | BIT(TWEN);
        break;
    case TW_BUS_ERROR:
        // some sort of erroneous state, prepare TWI to be readdressed
        printf("TWI_vect bus error\n");
        TWCR = 0;
        TWCR = BIT(TWIE) | BIT(TWINT) | BIT(TWEA) | BIT(TWEN);
        break;
    default:
        TWCR = BIT(TWIE) | BIT(TWINT) | BIT(TWEA) | BIT(TWEN);
        break;
    }
}
