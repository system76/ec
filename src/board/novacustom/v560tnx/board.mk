# SPDX-License-Identifier: GPL-3.0-only

board-y += board.c
board-y += gpio.c

EC = ite
CONFIG_EC_ITE_IT5570E = y

# Enable eSPI
CONFIG_BUS_ESPI = y

# Use S0ix
CFLAGS+=-DUSE_S0IX=1

# Include keyboard
KEYBOARD = 15in_102
KEYMAP=micmute

# Set keyboard LED mechanism
KBLED = rgb_pwm

# Set discrete GPU I2C bus
CFLAGS+=-DI2C_DGPU=I2C_1

# Set battery I2C bus
CFLAGS += -DI2C_SMBUS=I2C_4

# Set touchpad PS2 bus
CFLAGS += -DPS2_TOUCHPAD=PS2_3

# Set USB-PD I2C bus
CFLAGS+=-DI2C_USBPD=I2C_1

# Set smart charger parameters
CHARGER = oz26786
CFLAGS += \
	-DCHARGER_ADAPTER_RSENSE=5 \
	-DCHARGER_BATTERY_RSENSE=10 \
	-DCHARGER_CHARGE_CURRENT=1536 \
	-DCHARGER_CHARGE_VOLTAGE=17600 \
	-DCHARGER_INPUT_CURRENT=9000

# Set CPU power limits in watts and disable setting PSYS
# AC: power rating of the included AC adapter
# DC: battery discharge rate (1c discharge rate)
CFLAGS += \
	-DPOWER_LIMIT_PSYS_ENABLE=0 \
	-DPOWER_LIMIT_AC=180 \
	-DPOWER_LIMIT_DC=80

# 20V power supply
CFLAGS += -DAC_ADAPTER_VOLTAGE=20

# DGPU support
CFLAGS+=-DHAVE_DGPU=1 -DGPU_FAN1=3

# D-notifier support for NVIDIA
CFLAGS+= -DHAVE_D_NOTIFY=1 \
	-DD_NOTIFY_D1_MIN_W=180 \
	-DD_NOTIFY_D2_MIN_W=80 \
	-DD_NOTIFY_D3_MIN_W=75 \
	-DD_NOTIFY_D4_MIN_W=70 \
	-DD_NOTIFY_D5_MIN_W=65

# Set USB-PD parameters
USBPD=tps65987

# Add common code
include src/board/system76/common/common.mk
