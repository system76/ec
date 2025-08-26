# SPDX-License-Identifier: GPL-3.0-only

board-common-y += acpi.c
board-common-y += battery.c
board-common-y += config.c
board-common-$(CONFIG_HAVE_DGPU) += dgpu.c
board-common-y += ecpm.c
board-common-$(CONFIG_BUS_ESPI) += espi.c
board-common-y += fan.c
board-common-y += gctrl.c
board-common-y += kbc.c
board-common-y += kbscan.c
board-common-y += keymap.c
board-common-y += lid.c
board-common-y += main.c
board-common-y += parallel.c
board-common-y += pmc.c
board-common-y += pnp.c
board-common-y += ps2.c
board-common-y += pwm.c
board-common-y += scratch.c
board-common-$(CONFIG_SECURITY) += security.c
board-common-y += smbus.c
board-common-y += smfi.c
board-common-y += stdio.c
board-common-y += wireless.c

# Set log level
# 0 - NONE
# 1 - ERROR
# 2 - WARN
# 3 - INFO
# 4 - DEBUG
# 5 - TRACE
CFLAGS+=-DLEVEL=4

# Uncomment to enable debug logging over keyboard parallel port
#CFLAGS+=-DPARALLEL_DEBUG

CFLAGS += -DI2C_SMBUS=$(CONFIG_I2C_SMBUS)
# Uncomment to enable I2C debug on 0x76
#CFLAGS+=-DI2C_DEBUGGER=0x76

ifeq ($(CONFIG_SECURITY),y)
CFLAGS+=-DCONFIG_SECURITY=1
endif

ifeq ($(CONFIG_PLATFORM_INTEL),y)
board-common-y += peci.c
board-common-y += power/intel.c
CFLAGS += -DCONFIG_PLATFORM_INTEL=1
else ifeq ($(CONFIG_PLATFORM_AMD),y)
board-common-y += power/amd.c
CFLAGS += -DCONFIG_PLATFORM_AMD=1
else
$(error PLATFORM not specified)
endif

# Set CPU power limits
CFLAGS += -DPOWER_LIMIT_AC=$(CONFIG_POWER_LIMIT_AC)
CFLAGS += -DPOWER_LIMIT_DC=$(CONFIG_POWER_LIMIT_DC)

ifeq ($(CONFIG_BUS_ESPI),y)
CFLAGS += -DCONFIG_BUS_ESPI=1

# TODO: Use PECI over eSPI on all boards using eSPI
ifeq ($(CONFIG_PECI_OVER_ESPI),y)
CFLAGS += -DCONFIG_PECI_OVER_ESPI=1
endif
endif

ifeq ($(CONFIG_HAVE_DGPU),y)
CFLAGS += -DCONFIG_HAVE_DGPU=1
CFLAGS += -DI2C_DGPU=$(CONFIG_I2C_DGPU)
endif

# Include system76 common source
SYSTEM76_COMMON_DIR=src/board/system76/common
INCLUDE += $(SYSTEM76_COMMON_DIR)/common.mk
CFLAGS+=-I$(SYSTEM76_COMMON_DIR)/include

# Fan configs
ifneq ($(CONFIG_FAN1_PWM),)
CFLAGS += -DFAN1_PWM=$(CONFIG_FAN1_PWM)
ifneq ($(CONFIG_FAN1_PWM_MIN),)
CFLAGS += -DFAN1_PWM_MIN=$(CONFIG_FAN1_PWM_MIN)
endif
CFLAGS += -DBOARD_FAN1_POINTS=$(CONFIG_FAN1_POINTS)
endif

ifneq ($(CONFIG_FAN2_PWM),)
CFLAGS += -DFAN2_PWM=$(CONFIG_FAN2_PWM)
ifneq ($(CONFIG_FAN2_PWM_MIN),)
CFLAGS += -DFAN2_PWM_MIN=$(CONFIG_FAN2_PWM_MIN)
endif
CFLAGS += -DBOARD_FAN2_POINTS=$(CONFIG_FAN2_POINTS)
endif

# Set battery charging thresholds
BATTERY_START_THRESHOLD?=90
BATTERY_END_THRESHOLD?=100

CFLAGS+=\
	-DBATTERY_START_THRESHOLD=$(BATTERY_START_THRESHOLD) \
	-DBATTERY_END_THRESHOLD=$(BATTERY_END_THRESHOLD)

# Add charger
ifneq ($(CONFIG_CHARGER),)
board-common-y += charger/$(CONFIG_CHARGER).c
CFLAGS += -DCHARGER_ADAPTER_RSENSE=$(CONFIG_CHARGER_ADAPTER_RSENSE)
CFLAGS += -DCHARGER_BATTERY_RSENSE=$(CONFIG_CHARGER_BATTERY_RSENSE)
CFLAGS += -DCHARGER_CHARGE_CURRENT=$(CONFIG_CHARGER_CHARGE_CURRENT)
CFLAGS += -DCHARGER_CHARGE_VOLTAGE=$(CONFIG_CHARGER_CHARGE_VOLTAGE)
CFLAGS += -DCHARGER_INPUT_CURRENT=$(CONFIG_CHARGER_INPUT_CURRENT)
ifneq ($(CONFIG_CHARGER_PSYS_GAIN),)
CFLAGS += -DCHARGER_PSYS_GAIN=$(CONFIG_CHARGER_PSYS_GAIN)
endif
endif

# Add USB-PD
ifeq ($(CONFIG_HAVE_USBPD),y)
CFLAGS += -DCONFIG_HAVE_USBPD=1
board-common-$(CONFIG_USBPD_TPS65987) += usbpd/tps65987.c
CFLAGS += -DI2C_USBPD=$(CONFIG_I2C_USBPD)
endif

# Add keyboard
ifndef KEYBOARD
$(error KEYBOARD is not set by the board)
endif
KEYBOARD_DIR=src/keyboard/system76/$(KEYBOARD)
include $(KEYBOARD_DIR)/keyboard.mk

# Add kbled
ifeq ($(CONFIG_HAVE_KBLED),y)
CFLAGS += -DCONFIG_HAVE_KBLED=1
board-common-y += kbled/common.c
board-common-y += kbled/$(CONFIG_KBLED).c
ifneq ($(CONFIG_KBLED_DAC),)
CFLAGS += -DKBLED_DAC=$(CONFIG_KBLED_DAC)
endif
ifneq ($(CONFIG_I2C_KBLED),)
CFLAGS += -DI2C_KBLED=$(CONFIG_I2C_KBLED)
endif
endif

CFLAGS += -DPS2_TOUCHPAD=$(CONFIG_PS2_TOUCHPAD)

# Add scratch ROM
include $(SYSTEM76_COMMON_DIR)/scratch/scratch.mk

# Add scratch ROM for flash access
include $(SYSTEM76_COMMON_DIR)/flash/flash.mk
board-common-y += flash/wrapper.c

# Set external programmer
PROGRAMMER=$(wildcard /dev/serial/by-id/usb-Arduino*)

console_internal:
	cargo build --manifest-path tool/Cargo.toml --release
	sudo tool/target/release/system76_ectool console

console_external:
	sudo test -c "$(PROGRAMMER)"
	sleep 1 && echo C | sudo tee "$(PROGRAMMER)" &
	sudo tio -b 1000000 -m INLCRNL -t "$(PROGRAMMER)"

console_external_forced:
	sudo test -c "$(PROGRAMMER)"
	sleep 1 && echo F | sudo tee "$(PROGRAMMER)" &
	sudo tio -b 1000000 -m INLCRNL -t "$(PROGRAMMER)"

flash_internal: $(BUILD)/ec.rom
	cargo build --manifest-path tool/Cargo.toml --release
	sudo tool/target/release/system76_ectool flash $<

flash_external: $(BUILD)/ec.rom
	cargo build --manifest-path tools/ecflash/Cargo.toml --example isp --release
	sudo tools/ecflash/target/release/examples/isp $<
