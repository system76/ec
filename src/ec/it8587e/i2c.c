#include <board/cpu.h>
#include <ec/i2c.h>
#include <ec/smbus.h>

#define TIMEOUT (F_CPU/1000)

#define HOSTA_BYTE_DONE (1 << 7)
#define HOSTA_TIMEOUT (1 << 6)
#define HOSTA_NACK (1 << 5)
#define HOSTA_FAIL (1 << 4)
#define HOSTA_BUS_ERR (1 << 3)
#define HOSTA_DEV_ERR (1 << 2)
#define HOSTA_FINISH (1 << 1)
#define HOSTA_BUSY (1 << 0)
#define HOSTA_ERR (HOSTA_TIMEOUT | HOSTA_NACK | HOSTA_FAIL | HOSTA_BUS_ERR | HOSTA_DEV_ERR)

static void i2c_kill(void) {
    if (HOSTAA & HOSTA_BUSY) {
        HOCTLA = (1 << 1);
        // TODO: is timeout needed?
        while (HOSTAA & HOSTA_BUSY) {}
    }
    HOSTAA = HOSTAA;
    HOCTLA = 0;
    HOCTL2A = 0;
}

uint8_t i2c_transaction(uint8_t addr_rw, uint8_t *data, uint16_t length) {
    // End any current transaction
    i2c_kill();

    // Enable host controller with i2c compatibility
    HOCTL2A = (1 << 1) | 1;
	// Set slave address
	TRASLAA = addr_rw;

    static uint8_t __pdata status;
    static uint32_t __pdata timeout;
    static uint16_t __pdata i;
	for (i = 0; i < length; i++) {
        if ((addr_rw & 1) && ((i + 1) == length)) {
            // Set last byte
            HOCTLA |= (1 << 5);
        }
        // Clear status
        HOSTAA = HOSTAA;
        if (!(addr_rw & 1)) {
            // Set data byte
            HOBDBA = data[i];
        }
        if (i == 0) {
            // Start transaction
            HOCTLA = (1 << 6) | (0b111 << 2);
        }
        for(timeout = TIMEOUT; timeout > 0; timeout++) {
            status = HOSTAA;
            HOSTAA = status;
            if (status & HOSTA_ERR) {
                timeout = 0;
                break;
            }
            if (status & HOSTA_BYTE_DONE) {
                break;
            }
        }
        if (timeout == 0) {
            i2c_kill();
            return 1;
        }
        if (addr_rw & 1) {
            // Get data byte
            data[i] = HOBDBA;
        }
	}

    HOCTL2A = 0;

	return 0;
}
