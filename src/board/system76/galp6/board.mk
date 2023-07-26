# SPDX-License-Identifier: GPL-3.0-only

board-y += board.c
board-y += gpio.c

EC=ite
CONFIG_EC_ITE_IT5570E=y

# Enable eSPI
CONFIG_BUS_ESPI=y

# Enable firmware security
CONFIG_SECURITY=n

# Use S0ix
CFLAGS+=-DUSE_S0IX=1
# Apply PMC hack for S0ix
CFLAGS+=-DPMC_S0IX_HACK=1

# Include keyboard
KEYBOARD=14in_83

# Set keyboard LED mechanism
KBLED=white_dac
CFLAGS+=-DKBLED_DAC=2

# Set battery I2C bus
CFLAGS+=-DI2C_SMBUS=I2C_4

# Set touchpad PS2 bus
CFLAGS+=-DPS2_TOUCHPAD=PS2_3

# Set USB-PD I2C bus
CFLAGS+=-DI2C_USBPD=I2C_1

# Set smart charger parameters
# TODO: actually bq24800
# FIXME: Verify parts and values.
CFLAGS+=\
	-DCHARGER_ADAPTER_RSENSE=5 \
	-DCHARGER_BATTERY_RSENSE=10 \
	-DCHARGER_CHARGE_CURRENT=1536 \
	-DCHARGER_CHARGE_VOLTAGE=17400 \
	-DCHARGER_INPUT_CURRENT=4740

# Set CPU power limits in watts
CFLAGS+=\
	-DPOWER_LIMIT_AC=65 \
	-DPOWER_LIMIT_DC=45

# Set USB-PD parameters
USBPD=tps65987

# Add system76 common code
include src/board/system76/common/common.mk
