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

# Fan curve
CFLAGS+=-DBOARD_HEATUP=5
CFLAGS+=-DBOARD_COOLDOWN=20
CFLAGS+=-DBOARD_FAN_POINTS="\
	FAN_POINT(45, 25), \
	FAN_POINT(50, 30), \
	FAN_POINT(55, 30), \
	FAN_POINT(60, 40), \
	FAN_POINT(65, 50), \
	FAN_POINT(70, 60), \
	FAN_POINT(75, 70), \
	FAN_POINT(80, 80) \
"

# Add system76 common code
include src/board/system76/common/common.mk
