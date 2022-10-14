# SPDX-License-Identifier: GPL-3.0-only

board-y += main.c

EC=atmega
EC_VARIANT=atmega32u4

PORT=$(wildcard /dev/ttyACM*)
CONSOLE_BAUD=1000000
CFLAGS+=-D__CONSOLE_BAUD__=$(CONSOLE_BAUD)

console:
	sudo tio -b $(CONSOLE_BAUD) $(PORT)

flash: $(BUILD)/ec.ihx
	sudo avrdude -v -v -p $(EC_VARIANT) -c avr109 -P $(PORT) -b 115200 -D -U flash:w:$<:i
