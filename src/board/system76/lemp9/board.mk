EC=it5570e

# Add keymap to src
KEYMAP?=default
SRC+=$(BOARD_DIR)/keymap/$(KEYMAP).c

# Set log level
# 0 - NONE
# 1 - ERROR
# 2 - WARN
# 3 - INFO
# 4 - DEBUG
# 5 - TRACE
CFLAGS+=-DLEVEL=2

# Enable I2C debug on 0x76
CFLAGS+=-DI2C_DEBUGGER=0x76

# Set battery I2C bus
CFLAGS+=-DI2C_SMBUS=I2C_4

# Add scratch ROM source
SCRATCH_DIR=$(BOARD_DIR)/scratch
SCRATCH_SRC=$(wildcard $(SCRATCH_DIR)/*.c)
SCRATCH_INCLUDE=$(wildcard $(SCRATCH_DIR)/include/scratch/*.h) $(SCRATCH_DIR)/scratch.mk
SCRATCH_CFLAGS=-I$(SCRATCH_DIR)/include
include $(SCRATCH_DIR)/scratch.mk

# Include scratch header in main firmware
CFLAGS+=-I$(BUILD)/include
INCLUDE+=$(BUILD)/include/scratch.h

flash: $(BUILD)/ec.rom
	cargo build --manifest-path ecflash/Cargo.toml --example isp --release
	sudo ecflash/target/release/examples/isp --internal $<

isp: $(BUILD)/ec.rom
	cargo build --manifest-path ecflash/Cargo.toml --example isp --release
	sudo ecflash/target/release/examples/isp $<
