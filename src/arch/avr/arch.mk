# SPDX-License-Identifier: GPL-3.0-only

CC=avr-gcc -mmcu=$(EC_VARIANT)
CFLAGS+=-Os -fstack-usage -Wall -Werror -Wl,--gc-sections -Wl,-u,vfprintf -lprintf_flt
OBJCOPY = avr-objcopy
OBJ=$(sort $(patsubst src/%.c,$(BUILD)/%.o,$(SRC)))

# Run EC rom in simulator
sim: $(BUILD)/ec.elf
	simavr $< --mcu $(EC_VARIANT)

# Convert from Intel Hex file to binary file
$(BUILD)/ec.rom: $(BUILD)/ec.ihx
	@mkdir -p $(@D)
	$(OBJCOPY) -I ihex -O binary --gap-fill 0xFF $< $@

# Convert from ELF file to Intel Hex file
$(BUILD)/ec.ihx: $(BUILD)/ec.elf
	@mkdir -p $(@D)
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

# Link object files into ELF file
$(BUILD)/ec.elf: $(OBJ)
	@mkdir -p $(@D)
	$(CC) -o $@ $^

# Compile C files into object files
$(BUILD)/%.o: src/%.c $(INCLUDE)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ -c $<
