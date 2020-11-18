#include <asm/io.h>
#include <linux/module.h>
#include <linux/serial_core.h>

// This is the command region for System76 EC firmware. It must be
// enabled for LPC in the mainboard.
#define SYSTEM76_EC_BASE 0x0E00
#define SYSTEM76_EC_SIZE 256

#define REG_CMD 0
#define REG_RESULT 1

// When command register is 0, command is complete
#define CMD_FINISHED 0

// Print command. Registers are unique for each command
#define CMD_PRINT 4
#define CMD_PRINT_REG_FLAGS 2
#define CMD_PRINT_REG_LEN 3
#define CMD_PRINT_REG_DATA 4

static inline uint8_t system76_ec_read(uint8_t addr)
{
	return inb(SYSTEM76_EC_BASE + (uint16_t)addr);
}

static inline void system76_ec_write(uint8_t addr, uint8_t data)
{
	outb(data, SYSTEM76_EC_BASE + (uint16_t)addr);
}

void system76_ec_init(void)
{
	int i;

	// Clear entire command region
	for (i = 0; i < SYSTEM76_EC_SIZE; i++)
		system76_ec_write((uint8_t)i, 0);
}

void system76_ec_flush(void)
{
	system76_ec_write(REG_CMD, CMD_PRINT);

	// Wait for command completion
	while (system76_ec_read(REG_CMD) != CMD_FINISHED) {}

	system76_ec_write(CMD_PRINT_REG_LEN, 0);
}

void system76_ec_print(uint8_t byte)
{
	uint8_t len = system76_ec_read(CMD_PRINT_REG_LEN);
	system76_ec_write(CMD_PRINT_REG_DATA + len, byte);
	system76_ec_write(CMD_PRINT_REG_LEN, len + 1);

	// If we hit the end of the buffer, or were given a newline, flush
	if (byte == '\n' || len >= (SYSTEM76_EC_SIZE - CMD_PRINT_REG_DATA))
		system76_ec_flush();
}

static void system76_ec_console_write(struct console *co, const char *s, u_int count) {
  int i;
  for (i = 0; i < count; i++, s++) {
    system76_ec_print((uint8_t)*s);
  }
}

static int __init system76_ec_console_setup(struct console *co, char *options) {
	//TODO: actually detect EC
	system76_ec_init();

	return 0;
}

static struct console system76_ec_console = {
  .name    = "ttyec",
  .write   = system76_ec_console_write,
  .device  = uart_console_device,
  .setup   = system76_ec_console_setup,
  .flags   = CON_PRINTBUFFER,
  .index   = 0,
};


static int __init system76_ec_console_init(void) {
  register_console(&system76_ec_console);
  return 0;
}

console_initcall(system76_ec_console_init);

MODULE_LICENSE("GPL");
