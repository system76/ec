# SPDX-License-Identifier: GPL-3.0-only

EC=it8587e

# Add keymap to src
KEYMAP?=default
SRC+=$(BOARD_DIR)/keymap/$(KEYMAP).c

# Set battery I2C bus
CFLAGS+=-DI2C_SMBUS=I2C_0

# Set touchpad PS2 bus
CFLAGS+=-DPS2_TOUCHPAD=PS2_3

# Set smart charger parameters
CFLAGS+=\
	-DCHARGER_CHARGE_CURRENT=1536 \
	-DCHARGER_CHARGE_VOLTAGE=13056 \
	-DCHARGER_INPUT_CURRENT=1920

# Set battery charging thresholds
CFLAGS+=\
	-DBATTERY_START_THRESHOLD=0 \
	-DBATTERY_END_THRESHOLD=100

# Add system76 common code
include src/board/system76/common/common.mk
