# SPDX-License-Identifier: GPL-3.0-only

LVFS_DEVICE_ID = com.system76.galp5.ec.firmware
LVFS_DEVICE_NAME = "Galago Pro 5"
# TODO
LVFS_DEVICE_UUID = "00000000-0000-0000-0000-000000000000"

EC=ite
EC_VARIANT=it5570e

# Enable eSPI
CFLAGS+=-DEC_ESPI=1

# Use S0ix
CFLAGS+=-DUSE_S0IX=1

# Include keyboard
KEYBOARD=14in_83

# Set keyboard LED mechanism
KBLED=white_dac
CFLAGS+=-DKBLED_DAC=2

# Set discrete GPU I2C bus
CFLAGS+=-DI2C_DGPU=I2C_1

# Set battery I2C bus
CFLAGS+=-DI2C_SMBUS=I2C_4

# Set touchpad PS2 bus
CFLAGS+=-DPS2_TOUCHPAD=PS2_3

# Set smart charger parameters
# FIXME: Verify parts and values.
CHARGER=oz26786
CFLAGS+=\
	-DCHARGER_ADAPTER_RSENSE=10 \
	-DCHARGER_BATTERY_RSENSE=10 \
	-DCHARGER_CHARGE_CURRENT=1536 \
	-DCHARGER_CHARGE_VOLTAGE=17400 \
	-DCHARGER_INPUT_CURRENT=3420

# Set CPU power limits in watts
CFLAGS+=\
	-DPOWER_LIMIT_AC=65 \
	-DPOWER_LIMIT_DC=28

# Custom fan curve
CFLAGS+=-DBOARD_FAN_POINTS="\
	FAN_POINT(70, 25), \
	FAN_POINT(80, 25), \
	FAN_POINT(80, 40), \
	FAN_POINT(88, 40), \
	FAN_POINT(88, 100) \
"

# DGPU support
CFLAGS+=-DHAVE_DGPU=1
CFLAGS+=-DBOARD_DGPU_FAN_POINTS="\
	FAN_POINT(70, 25), \
	FAN_POINT(75, 40), \
	FAN_POINT(80, 75), \
	FAN_POINT(85, 90), \
	FAN_POINT(90, 100) \
"

# Add system76 common code
include src/board/system76/common/common.mk
