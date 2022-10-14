# SPDX-License-Identifier: GPL-3.0-only

-include config.mk

# Parameter for current board
ifeq ($(BOARD),)
all:
	@echo "Please set BOARD to one of the following:"
	@cd src/board && for board in */*/board.mk; do \
		echo "  $$(dirname "$$board")"; \
	done
	@exit 1
else
# Calculate version
DATE=$(shell git show --format="%cd" --date="format:%Y-%m-%d" --no-patch)
REV=$(shell git describe --always --dirty)
VERSION?=$(DATE)_$(REV)

# Set build directory
BUILD=build/$(BOARD)/$(VERSION)

# Default target - build the board's EC firmware
all: $(BUILD)/ec.rom
	$(info Built '$(VERSION)' for '$(BOARD)')

# Include common source
COMMON_DIR=src/common
INCLUDE=$(wildcard $(COMMON_DIR)/include/common/*.h) $(COMMON_DIR)/common.mk
CFLAGS=-I$(COMMON_DIR)/include -D__FIRMWARE_VERSION__=$(VERSION)
include $(COMMON_DIR)/common.mk
SRC += $(foreach src, $(common-y), $(COMMON_DIR)/$(src))

# Include the board's source
BOARD_DIR=src/board/$(BOARD)
INCLUDE+=$(wildcard $(BOARD_DIR)/include/board/*.h) $(BOARD_DIR)/board.mk
CFLAGS+=-I$(BOARD_DIR)/include -D__BOARD__=$(BOARD)
include $(BOARD_DIR)/board.mk
SRC += $(foreach src, $(board-y), $(BOARD_DIR)/$(src))
SRC += $(foreach src, $(board-common-y), $(SYSTEM76_COMMON_DIR)/$(src))
SRC += $(foreach src, $(keyboard-y), $(KEYBOARD_DIR)/$(src))

# The board will define the embedded controller
# Include the embedded controller's source
EC_DIR=src/ec/$(EC)
INCLUDE+=$(wildcard $(EC_DIR)/include/ec/*.h) $(EC_DIR)/ec.mk
CFLAGS+=-I$(EC_DIR)/include -D__EC__=$(EC_VARIANT) -D$(EC_VARIANT)
include $(EC_DIR)/ec.mk
SRC += $(foreach src, $(ec-y), $(EC_DIR)/$(src))

# The EC will define the architecture
# Include the architecture's source
ARCH_DIR=src/arch/$(ARCH)
INCLUDE+=$(wildcard $(ARCH_DIR)/include/arch/*.h) $(ARCH_DIR)/arch.mk
CFLAGS+=-I$(ARCH_DIR)/include -D__ARCH__=$(ARCH)
include $(ARCH_DIR)/arch.mk
SRC += $(foreach src, $(arch-y), $(ARCH_DIR)/$(src))

include $(ARCH_DIR)/toolchain.mk

# The architecture defines build targets, no more is required
endif

# Target to remove build artifacts
clean:
	rm -rf build
