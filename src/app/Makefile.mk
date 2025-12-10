# SPDX-License-Identifier: GPL-3.0-only
# SPDX-FileCopyrightText: 2020 System76, Inc.

app-y += acpi.c
app-y += battery.c
app-y += config.c
app-y += ecpm.c
app-$(CONFIG_BUS_ESPI) += espi.c
app-y += fan.c
app-y += kbc.c
app-y += kbscan.c
app-y += keymap.c
app-y += lid.c
app-y += main.c
app-y += parallel.c
app-y += pmc.c
app-y += pnp.c
app-y += pwm.c
app-y += scratch.c
app-y += smbus.c
app-y += smfi.c
app-y += stdio.c
app-y += wireless.c

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

ifeq ($(CONFIG_PLATFORM_INTEL),y)
app-y += peci.c
app-y += power/intel.c
CFLAGS += -DCONFIG_PLATFORM_INTEL=1
else ifeq ($(CONFIG_PLATFORM_AMD),y)
app-y += power/amd.c
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
app-y += charger/$(CONFIG_CHARGER).c
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
app-$(CONFIG_USBPD_TPS65987) += usbpd/tps65987.c
CFLAGS += -DI2C_USBPD=$(CONFIG_I2C_USBPD)
endif

# Add keyboard
ifndef KEYBOARD
$(error KEYBOARD is not set by the board)
endif
KEYBOARD_DIR=src/keyboard/system76/$(KEYBOARD)
include $(KEYBOARD_DIR)/Makefile.mk

CFLAGS += -DPS2_TOUCHPAD=$(CONFIG_PS2_TOUCHPAD)

# Add scratch ROM
include $(APP_DIR)/scratch/Makefile.mk

# Add scratch ROM for flash access
include $(APP_DIR)/flash/Makefile.mk
app-y += flash/wrapper.c

# Set external programmer
PROGRAMMER=$(wildcard /dev/serial/by-id/usb-Arduino*)

console_internal:
	cargo build --manifest-path tools/system76_ectool/Cargo.toml --release
	sudo tools/system76_ectool/target/release/system76_ectool console

console_external:
	sudo test -c "$(PROGRAMMER)"
	sleep 1 && echo C | sudo tee "$(PROGRAMMER)" &
	sudo tio -b 1000000 -m INLCRNL -t "$(PROGRAMMER)"

console_external_forced:
	sudo test -c "$(PROGRAMMER)"
	sleep 1 && echo F | sudo tee "$(PROGRAMMER)" &
	sudo tio -b 1000000 -m INLCRNL -t "$(PROGRAMMER)"

flash_internal: $(BUILD)/ec.rom
	cargo build --manifest-path tools/system76_ectool/Cargo.toml --release
	sudo tools/system76_ectool/target/release/system76_ectool flash $<

flash_external: $(BUILD)/ec.rom
	cargo build --manifest-path tools/ecflash/Cargo.toml --example isp --release
	sudo tools/ecflash/target/release/examples/isp $<
