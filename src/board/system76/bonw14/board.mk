# SPDX-License-Identifier: GPL-3.0-only

EC=ite
EC_VARIANT=it5570e

# Include keyboard
KEYBOARD=15in_102_nkey

# Set keyboard LED mechanism
KBLED=bonw14

# Set discrete GPU I2C bus
CFLAGS+=-DI2C_DGPU=I2C_1

# Set battery I2C bus
CFLAGS+=-DI2C_SMBUS=I2C_4

# Set touchpad PS2 bus
CFLAGS+=-DPS2_TOUCHPAD=PS2_3

# Set smart charger parameters
CFLAGS+=\
	-DCHARGER_ADAPTER_RSENSE=5 \
	-DCHARGER_BATTERY_RSENSE=10 \
	-DCHARGER_CHARGE_CURRENT=3072 \
	-DCHARGER_CHARGE_VOLTAGE=16800 \
	-DCHARGER_INPUT_CURRENT=14000

# Set CPU power limits in watts
CFLAGS+=\
	-DPOWER_LIMIT_AC=180 \
	-DPOWER_LIMIT_DC=28

# Custom fan curve
CFLAGS+=-DBOARD_FAN_POINTS="\
	FAN_POINT(60, 40), \
	FAN_POINT(65, 60), \
	FAN_POINT(70, 75), \
	FAN_POINT(75, 90), \
	FAN_POINT(80, 100) \
"

# Enable DGPU support
CFLAGS+=-DHAVE_DGPU=1
CFLAGS+=-DBOARD_DGPU_FAN_POINTS="\
	FAN_POINT(60, 40), \
	FAN_POINT(65, 60), \
	FAN_POINT(70, 75), \
	FAN_POINT(75, 90), \
	FAN_POINT(80, 100) \
"

# Add system76 common code
include src/board/system76/common/common.mk
