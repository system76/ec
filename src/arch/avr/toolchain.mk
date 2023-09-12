# SPDX-License-Identifier: GPL-3.0-only

CC = avr-gcc -mmcu=$(EC_VARIANT)
CFLAGS += -MMD -Os -fstack-usage -Wall -Werror \
	-Wl,--gc-sections -Wl,-u,vfprintf -lprintf_flt

# https://gcc.gnu.org/bugzilla/show_bug.cgi?id=105523
_gcc_version = $(shell avr-gcc --version 2>/dev/null)
ifneq ($(findstring 12.,$(_gcc_version)),)
CFLAGS += --param=min-pagesize=0
else ifneq ($(findstring 13.,$(_gcc_version)),)
CFLAGS += --param=min-pagesize=0
endif

OBJCOPY = avr-objcopy

OBJ=$(sort $(patsubst src/%.c,$(BUILD)/%.o,$(SRC)))

# Run EC rom in simulator
sim: $(BUILD)/ec.elf
	simavr $< --mcu $(EC_VARIANT)

# Convert from Intel Hex file to binary file
$(BUILD)/ec.rom: $(BUILD)/ec.ihx
	@echo "  OBJCOPY   $(subst $(obj)/,,$@)"
	mkdir -p $(@D)
	$(OBJCOPY) -I ihex -O binary --gap-fill 0xFF $< $@

# Convert from ELF file to Intel Hex file
$(BUILD)/ec.ihx: $(BUILD)/ec.elf
	@echo "  OBJCOPY   $(subst $(obj)/,,$@)"
	mkdir -p $(@D)
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

# Link object files into ELF file
$(BUILD)/ec.elf: $(OBJ)
	@echo "  LINK      $(subst $(obj)/,,$@)"
	mkdir -p $(@D)
	$(CC) -o $@ $^

# Compile C files into object files
$(BUILD)/%.o: src/%.c $(INCLUDE)
	@echo "  CC        $(subst $(obj)/,,$@)"
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ -c $<

# Add dependency rules
DEP = $(OBJ:%.o=%.d)
-include $(DEP)
