# SPDX-License-Identifier: GPL-3.0-only

EC=it5570e

# Enable eSPI
CFLAGS+=-DEC_ESPI=1

# Include keyboard
KEYBOARD=15in_102

# Set keyboard LED mechanism
KBLED=rgb_pwm

# Set battery I2C bus
CFLAGS+=-DI2C_SMBUS=I2C_4

# Set touchpad PS2 bus
CFLAGS+=-DPS2_TOUCHPAD=PS2_3

# Set smart charger parameters
# Adapter input current = 4.74A
# PRS1 = 0.010 ohm. Divide adapter input current by 2.
# PRS2 = 0.010 ohm. Use desired charge current.
CHARGER=oz26786
CFLAGS+=\
	-DCHARGER_CHARGE_CURRENT=0xB80 \
	-DCHARGER_CHARGE_VOLTAGE=8800 \
	-DCHARGER_INPUT_CURRENT=0x900

# Add system76 common code
include src/board/system76/common/common.mk
