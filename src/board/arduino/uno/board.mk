EC=atmega328p

PORT=$(wildcard /dev/ttyACM*)

console:
	sudo tio -b 9600 $(PORT)

flash: $(BUILD)/ec.ihx
	sudo avrdude -v -v -p $(EC) -c arduino -P $(PORT) -b 115200 -D -U flash:w:$<:i
