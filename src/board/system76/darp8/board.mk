# SPDX-License-Identifier: GPL-3.0-only

board-y += board.c
board-y += gpio.c

EC=ite
CONFIG_EC_ITE_IT5570E=y

# Enable eSPI
CFLAGS+=-DEC_ESPI=1
# Apply PMC hack for S0ix
CFLAGS+=-DPMC_S0IX_HACK=1

# Use S0ix
CFLAGS+=-DUSE_S0IX=1

# Include keyboard
KEYBOARD=15in_102

# Set keyboard LED mechanism
KBLED=rgb_pwm

# Set battery I2C bus
CFLAGS+=-DI2C_SMBUS=I2C_4

# Set touchpad PS2 bus
CFLAGS+=-DPS2_TOUCHPAD=PS2_3

# Set smart charger parameters
CHARGER=oz26786
CFLAGS+=\
	-DCHARGER_ADAPTER_RSENSE=10 \
	-DCHARGER_BATTERY_RSENSE=10 \
	-DCHARGER_CHARGE_CURRENT=3072 \
	-DCHARGER_CHARGE_VOLTAGE=8800 \
	-DCHARGER_INPUT_CURRENT=4740

# Add system76 common code
include src/board/system76/common/common.mk
