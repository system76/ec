# SPDX-License-Identifier: GPL-3.0-only

EC=it5570e

# Enable eSPI
CFLAGS+=-DEC_ESPI=1

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
# Adapter input current = 3.42A
# PRS1 = 0.005 ohm. Divide adapter input current by 4.
# PRS2 = 0.005 ohm. Divide charge current by 2.
CHARGER=oz26786
CFLAGS+=\
	-DCHARGER_CHARGE_CURRENT=0x600 \
	-DCHARGER_CHARGE_VOLTAGE=8800 \
	-DCHARGER_INPUT_CURRENT=0x300

# Add system76 common code
include src/board/system76/common/common.mk
