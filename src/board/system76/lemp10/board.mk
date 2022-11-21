# SPDX-License-Identifier: GPL-3.0-only

board-y += board.c
board-y += gpio.c
board-y += interrupts.c

EC=ite
CONFIG_EC_ITE_IT5570E=y

# Enable eSPI
CONFIG_BUS_ESPI=y

# Use S0ix
CFLAGS+=-DUSE_S0IX=1

# Include keyboard
KEYBOARD=14in_83

# Set keyboard LED mechanism
KBLED=white_dac
CFLAGS+=-DKBLED_DAC=2

# Set battery I2C bus
CFLAGS+=-DI2C_SMBUS=I2C_4

# Set touchpad PS2 bus
CFLAGS+=-DPS2_TOUCHPAD=PS2_3

# Set smart charger parameters
CFLAGS+=\
	-DCHARGER_ADAPTER_RSENSE=5 \
	-DCHARGER_BATTERY_RSENSE=5 \
	-DCHARGER_CHARGE_CURRENT=3072 \
	-DCHARGER_CHARGE_VOLTAGE=8800 \
	-DCHARGER_INPUT_CURRENT=3420

# Add system76 common code
include src/board/system76/common/common.mk
