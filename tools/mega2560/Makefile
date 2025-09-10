# SPDX-License-Identifier: GPL-3.0-only
# SPDX-FileCopyrightText: 2025 System76, Inc.

# Disable built-in rules and variables
MAKEFLAGS += --no-builtin-rules --no-builtin-variables

# Default to silent builds
ifndef VERBOSE
MAKEFLAGS += --silent
.SILENT:
endif

PROGRAM = mega2560
MCU = atmega2560

CC = avr-gcc
CFLAGS = -MMD -Os -Wall -Werror
CFLAGS += -mmcu=$(MCU) -DF_CPU=16000000
CFLAGS += -Wl,--gc-sections -Wl,-u,vfprintf -lprintf_flt -fstack-usage
CFLAGS += -Isrc/
LDFLAGS += -mmcu=$(MCU)
OBJCOPY = avr-objcopy

SRC = \
	src/gpio.c \
	src/main.c \
	src/parallel.c \
	src/uart.c

# https://gcc.gnu.org/bugzilla/show_bug.cgi?id=105523
_gcc_version = $(shell avr-gcc --version 2>/dev/null)
ifneq ($(findstring 12.,$(_gcc_version)),)
CFLAGS += --param=min-pagesize=0
else ifneq ($(findstring 13.,$(_gcc_version)),)
CFLAGS += --param=min-pagesize=0
endif

PORT = $(wildcard /dev/ttyACM*)
CONSOLE_BAUD = 1000000
CFLAGS += -D__CONSOLE_BAUD__=$(CONSOLE_BAUD)

BUILD_DIR := build

OBJ = $(sort $(patsubst src/%.c,$(BUILD_DIR)/%.o,$(SRC)))

all: $(BUILD_DIR)/$(PROGRAM).bin

# Convert from ELF file to binary file
$(BUILD_DIR)/$(PROGRAM).bin: $(BUILD_DIR)/$(PROGRAM).elf
	@echo "  OBJCOPY   $(subst $(BUILD_DIR)/,,$@)"
	mkdir -p $(@D)
	$(OBJCOPY) -O binary --gap-fill 0xFF $< $@

# Link object files into ELF file
$(BUILD_DIR)/$(PROGRAM).elf: $(OBJ)
	@echo "  LINK      $(subst $(BUILD_DIR)/,,$@)"
	mkdir -p $(@D)
	$(CC) $(LDFLAGS) -o $@ $^

# Compile C files into object files
$(BUILD_DIR)/%.o: src/%.c $(INCLUDE)
	@echo "  CC        $(subst $(BUILD_DIR)/,,$@)"
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ -c $<

# Run firmware ROM in simulator
.PHONY: sim
sim: $(BUILD_DIR)/$(PROGRAM).elf
	simavr $< --mcu $(MCU)

.PHONY: console
console:
	sudo tio -b $(CONSOLE_BAUD) -m INLCRNL $(PORT)

# Flash firmware to the mega2560
.PHONY: flash
flash: $(BUILD_DIR)/$(PROGRAM).elf
	sudo avrdude -v -v -p $(MCU) -c wiring -P $(PORT) -b 115200 -D -U $<

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

# Add dependency rules
DEP = $(OBJ:%.o=%.d)
-include $(DEP)
