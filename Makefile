# Parameter for current board
#BOARD?=system76/galp3-c
#BOARD?=arduino/mega2560
#BOARD?=arduino/micro
BOARD?=arduino/uno

# Set build directory
BUILD=build/$(BOARD)

# Default target - build the board's EC firmware
all: $(BUILD)/ec.rom

# Target to remove build artifacts
clean:
	rm -rf build

# Include common source
SRC=$(wildcard src/*.c)
INCLUDE=$(wildcard src/include/common/*.h)
CFLAGS=-Isrc/include

# Include the board's source
BOARD_DIR=src/board/$(BOARD)
SRC+=$(wildcard $(BOARD_DIR)/*.c)
INCLUDE+=$(wildcard $(BOARD_DIR)/include/board/*.h)
CFLAGS+=-I$(BOARD_DIR)/include
include $(BOARD_DIR)/board.mk

# The board will define the embedded controller
# Include the embedded controller's source
EC_DIR=src/ec/$(EC)
SRC+=$(wildcard $(EC_DIR)/*.c)
INCLUDE+=$(wildcard $(EC_DIR)/include/ec/*.h)
CFLAGS+=-I$(EC_DIR)/include
include $(EC_DIR)/ec.mk

# The EC will define the architecture
# Include the architecture's source
ARCH_DIR=src/arch/$(ARCH)
SRC+=$(wildcard $(ARCH_DIR)/*.c)
INCLUDE+=$(wildcard $(ARCH_DIR)/include/arch/*.h)
CFLAGS+=-I$(ARCH_DIR)/include
include $(ARCH_DIR)/arch.mk

# The architecture defines build targets, no more is required
