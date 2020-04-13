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
CFLAGS+=-DLEVEL=4

# Enable I2C debug on 0x76
#CFLAGS+=-DI2C_DEBUGGER=0x76

# Set battery I2C bus
CFLAGS+=-DI2C_SMBUS=I2C_4

# Set touchpad PS2 bus
CFLAGS+=-DPS2_TOUCHPAD=PS2_3

# Set smart charger parameters
#TODO: Find out why input current must by divided by two
CFLAGS+=\
	-DCHARGER_CHARGE_CURRENT=1536 \
	-DCHARGER_CHARGE_VOLTAGE=8800 \
	-DCHARGER_INPUT_CURRENT=1600

# Enable debug logging over keyboard parallel port
#CFLAGS+=-DPARPORT_DEBUG

# Set scratch ROM parameters
SCRATCH_OFFSET=1024
SCRATCH_SIZE=1024
CFLAGS+=-DSCRATCH_OFFSET=$(SCRATCH_OFFSET) -DSCRATCH_SIZE=$(SCRATCH_SIZE)

# Copy parameters to use when compiling scratch ROM
SCRATCH_INCLUDE=$(INCLUDE)
SCRATCH_CFLAGS=$(CFLAGS)

# Add scratch ROM source
SCRATCH_DIR=$(BOARD_DIR)/scratch
SCRATCH_SRC=$(wildcard $(SCRATCH_DIR)/*.c)
SCRATCH_INCLUDE+=$(wildcard $(SCRATCH_DIR)/include/scratch/*.h) $(SCRATCH_DIR)/scratch.mk
SCRATCH_CFLAGS+=-I$(SCRATCH_DIR)/include -D__SCRATCH__
include $(SCRATCH_DIR)/scratch.mk

# Include scratch header in main firmware
CFLAGS+=-I$(BUILD)/include
INCLUDE+=$(BUILD)/include/scratch.h

console_internal:
	cargo build --manifest-path tool/Cargo.toml --release
	sudo tool/target/release/system76_ectool console

console_external:
	sleep 1 && echo C | sudo tee /dev/ttyACM* &
	sudo tio -b 1000000 -m INLCRNL /dev/ttyACM*

flash_internal: $(BUILD)/ec.rom
	cargo build --manifest-path tool/Cargo.toml --release
	sudo tool/target/release/system76_ectool flash $<

flash_external: $(BUILD)/ec.rom
	cargo build --manifest-path ecflash/Cargo.toml --example isp --release
	sudo ecflash/target/release/examples/isp $<
