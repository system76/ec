# SPDX-License-Identifier: GPL-3.0-only

board-y += board.c
board-y += gpio.c

EC = ite
CONFIG_EC_ITE_IT5570E = y
CONFIG_EC_FLASH_SIZE_256K = y

# Enable eSPI
CONFIG_BUS_ESPI = y
CONFIG_PECI_OVER_ESPI = y

# Enable firmware security
CONFIG_SECURITY=y

# Include keyboard
KEYBOARD = 18H9LHA04

# Set keyboard LED mechanism
CONFIG_HAVE_KBLED = y
KBLED = rgb_pwm

# Set touchpad PS2 bus
CFLAGS += -DPS2_TOUCHPAD=PS2_3

# Set smart charger parameters
CHARGER = oz26786
CFLAGS += -DI2C_SMBUS=I2C_4
CFLAGS+=\
	-DCHARGER_ADAPTER_RSENSE=10 \
	-DCHARGER_BATTERY_RSENSE=10 \
	-DCHARGER_CHARGE_CURRENT=3072 \
	-DCHARGER_CHARGE_VOLTAGE=17600 \
	-DCHARGER_INPUT_CURRENT=11500

# Set USB-PD parameters
CONFIG_HAVE_USBPD = y
CONFIG_USBPD_TPS65987 = y
CFLAGS += -DI2C_USBPD=I2C_1

# Set CPU power limits in watts
CFLAGS += \
	-DPOWER_LIMIT_AC=230 \
	-DPOWER_LIMIT_DC=55

# Custom fan curve
CFLAGS += -DFAN1_PWM=DCR2
CFLAGS += -DBOARD_FAN1_HEATUP=5
CFLAGS += -DBOARD_FAN1_COOLDOWN=20
CFLAGS += -DBOARD_FAN1_POINTS="\
	FAN_POINT(50, 30), \
	FAN_POINT(60, 30), \
	FAN_POINT(65, 40), \
	FAN_POINT(75, 60), \
	FAN_POINT(80, 75), \
	FAN_POINT(85, 90), \
	FAN_POINT(90, 100), \
"

# Enable DGPU support
CONFIG_HAVE_DGPU = y
CFLAGS += -DI2C_DGPU=I2C_1

CFLAGS += -DFAN2_PWM=DCR4
CFLAGS += -DBOARD_FAN2_HEATUP=5
CFLAGS += -DBOARD_FAN2_COOLDOWN=20
CFLAGS += -DBOARD_FAN2_POINTS="\
	FAN_POINT(50, 30), \
	FAN_POINT(60, 30), \
	FAN_POINT(65, 40), \
	FAN_POINT(75, 60), \
	FAN_POINT(80, 75), \
	FAN_POINT(85, 90), \
	FAN_POINT(90, 100), \
"

# Add system76 common code
include src/board/system76/common/common.mk
