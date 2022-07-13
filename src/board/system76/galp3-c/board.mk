# SPDX-License-Identifier: GPL-3.0-only

EC=it8587e

# Include keyboard
KEYBOARD=14in_86

# Set keyboard LED mechanism
KBLED=white_dac
CFLAGS+=-DKBLED_DAC=5

# Set battery I2C bus
CFLAGS+=-DI2C_SMBUS=I2C_0

# Set touchpad PS2 bus
CFLAGS+=-DPS2_TOUCHPAD=PS2_3

# Set smart charger parameters
# FIXME: Verify parts and values.
CFLAGS+=\
	-DCHARGER_ADAPTER_RSENSE=10 \
	-DCHARGER_BATTERY_RSENSE=10 \
	-DCHARGER_CHARGE_CURRENT=1536 \
	-DCHARGER_CHARGE_VOLTAGE=13056 \
	-DCHARGER_INPUT_CURRENT=2100

# Add system76 common code
include src/board/system76/common/common.mk
