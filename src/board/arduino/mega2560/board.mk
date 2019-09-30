EC=atmega2560

PORT=$(wildcard /dev/ttyACM*)

console:
	sudo tio -b 9600 $(PORT)

flash: $(BUILD)/ec.ihx
	sudo avrdude -v -v -p $(EC) -c wiring -P $(PORT) -b 115200 -D -U flash:w:$<:i
