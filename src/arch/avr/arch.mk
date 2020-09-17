# SPDX-License-Identifier: GPL-3.0-only

CC=avr-gcc -mmcu=$(EC)
CFLAGS+=-Os -fstack-usage -Wall -Werror -Wl,--gc-sections -Wl,-u,vfprintf -lprintf_flt
OBJ=$(patsubst src/%.c,$(BUILD)/%.o,$(SRC))

# Run EC rom in simulator
sim: $(BUILD)/ec.elf
	simavr $< --mcu $(EC)

# Convert from Intel Hex file to binary file
$(BUILD)/ec.rom: $(BUILD)/ec.ihx
	@mkdir -p $(@D)
	makebin -p < $< > $@

# Convert from ELF file to Intel Hex file
$(BUILD)/ec.ihx: $(BUILD)/ec.elf
	@mkdir -p $(@D)
	avr-objcopy -j .text -j .data -O ihex $< $@

# Link object files into ELF file
$(BUILD)/ec.elf: $(OBJ)
	@mkdir -p $(@D)
	$(CC) -o $@ $^

# Compile C files into object files
$(BUILD)/%.o: src/%.c $(INCLUDE)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ -c $<
