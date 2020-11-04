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
SRC=$(wildcard $(COMMON_DIR)/*.c)
INCLUDE=$(wildcard $(COMMON_DIR)/include/common/*.h) $(COMMON_DIR)/common.mk
CFLAGS=-I$(COMMON_DIR)/include -D__FIRMWARE_VERSION__=$(VERSION) -D__FIRMWARE_DATE__=$(DATE)
include $(COMMON_DIR)/common.mk

# Include the board's source
BOARD_DIR=src/board/$(BOARD)
SRC+=$(wildcard $(BOARD_DIR)/*.c)
INCLUDE+=$(wildcard $(BOARD_DIR)/include/board/*.h) $(BOARD_DIR)/board.mk
CFLAGS+=-I$(BOARD_DIR)/include -D__BOARD__=$(BOARD)
include $(BOARD_DIR)/board.mk

# The board will define the embedded controller
# Include the embedded controller's source
EC_DIR=src/ec/$(EC)
SRC+=$(wildcard $(EC_DIR)/*.c)
INCLUDE+=$(wildcard $(EC_DIR)/include/ec/*.h) $(EC_DIR)/ec.mk
CFLAGS+=-I$(EC_DIR)/include -D__EC__=$(EC)
include $(EC_DIR)/ec.mk

# The EC will define the architecture
# Include the architecture's source
ARCH_DIR=src/arch/$(ARCH)
SRC+=$(wildcard $(ARCH_DIR)/*.c)
INCLUDE+=$(wildcard $(ARCH_DIR)/include/arch/*.h) $(ARCH_DIR)/arch.mk
CFLAGS+=-I$(ARCH_DIR)/include -D__ARCH__=$(ARCH)
include $(ARCH_DIR)/arch.mk

# The architecture defines build targets, no more is required
endif

# Target to remove build artifacts
clean:
	rm -rf build
