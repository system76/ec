# SPDX-License-Identifier: GPL-3.0-only

EC=it5570e

# Add keymap to src
KEYMAP?=default
SRC+=$(BOARD_DIR)/keymap/$(KEYMAP).c

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

# Set battery charging thresholds
CFLAGS+=\
	-DBATTERY_START_THRESHOLD=0 \
	-DBATTERY_END_THRESHOLD=100

# Add system76 common code
include src/board/system76/common/common.mk
