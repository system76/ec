#include <stdio.h>
#include <avr/io.h>
#include <util/twi.h>

#include <board/cpu.h>
#include <common/i2c.h>

#define TIMEOUT (F_CPU/1000)

uint8_t i2c_start(uint8_t addr, bool read) {
	uint32_t count;

	// reset TWI control register
	TWCR = 0;
	// transmit START condition
	TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN);
	// wait for end of transmission
	count = TIMEOUT;
	while(!(TWCR & (1<<TWINT)) && count > 0) count -= 1;
	if (count == 0) {
		printf("i2c_start timed out waiting to send start to 0x%X\n", addr);
		return 1;
	}

	// check if the start condition was successfully transmitted
	if((TWSR & 0xF8) != TW_START){
		printf("i2c_start failed to send start condition to 0x%X\n", addr);
		return 1;
	}

	// load slave addr into data register
	TWDR = ((addr << 1) | read);
	// start transmission of addr
	TWCR = (1<<TWINT) | (1<<TWEN);
	// wait for end of transmission
	count = TIMEOUT;
	while(!(TWCR & (1<<TWINT)) && count > 0) count -= 1;
	if (count == 0) {
		printf("i2c_start timed out waiting to send addr to 0x%X\n", addr);
		return 1;
	}

	// check if the device has acknowledged the READ / WRITE mode
	uint8_t twst = TW_STATUS & 0xF8;
	if ((twst != TW_MT_SLA_ACK) && (twst != TW_MR_SLA_ACK)) {
		printf("i2c_start failed to receive ack from 0x%X\n", addr);
		return 1;
	}

	return 0;
}

void i2c_stop(void) {
	// transmit STOP condition
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
}

uint8_t i2c_write(uint8_t data) {
	uint32_t count;

	// load data into data register
	TWDR = data;
	// start transmission of data
	TWCR = (1<<TWINT) | (1<<TWEN);
	// wait for end of transmission
	count = TIMEOUT;
	while(!(TWCR & (1<<TWINT)) && count > 0) count -= 1;
	if (count == 0) {
		printf("i2c_write timed out waiting to send 0x%X\n", data);
		return 1;
	}

	if( (TWSR & 0xF8) != TW_MT_DATA_ACK ){
		printf("i2c_write failed to receive ack for 0x%X\n", data);
		return 1;
	}

	return 0;
}

uint8_t i2c_read(bool ack) {
    if (ack) {
    	// start TWI module and acknowledge data after reception
    	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWEA);
    } else {
        // start receiving without acknowledging reception
    	TWCR = (1<<TWINT) | (1<<TWEN);
    }
	// wait for end of transmission
	while( !(TWCR & (1<<TWINT)) );
	// return received data from TWDR
	return TWDR;
}
