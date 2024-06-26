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
board-common-y += peci.c
board-common-y += pmc.c
board-common-y += pnp.c
board-common-y += power.c
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

# Uncomment to enable I2C debug on 0x76
#CFLAGS+=-DI2C_DEBUGGER=0x76

ifeq ($(CONFIG_SECURITY),y)
CFLAGS+=-DCONFIG_SECURITY=1
endif

# Set external programmer
PROGRAMMER=$(wildcard /dev/serial/by-id/usb-Arduino*)

ifeq ($(CONFIG_BUS_ESPI),y)
CFLAGS += -DCONFIG_BUS_ESPI=1

# TODO: Use PECI over eSPI on all boards using eSPI
ifeq ($(CONFIG_PECI_OVER_ESPI),y)
CFLAGS += -DCONFIG_PECI_OVER_ESPI=1
endif
endif

ifeq ($(CONFIG_HAVE_DGPU),y)
CFLAGS += -DCONFIG_HAVE_DGPU=1
endif

# Include system76 common source
SYSTEM76_COMMON_DIR=src/board/system76/common
INCLUDE += $(SYSTEM76_COMMON_DIR)/common.mk
CFLAGS+=-I$(SYSTEM76_COMMON_DIR)/include

# Set battery charging thresholds
BATTERY_START_THRESHOLD?=90
BATTERY_END_THRESHOLD?=100

CFLAGS+=\
	-DBATTERY_START_THRESHOLD=$(BATTERY_START_THRESHOLD) \
	-DBATTERY_END_THRESHOLD=$(BATTERY_END_THRESHOLD)

# Add charger
CHARGER?=bq24780s
board-common-y += charger/$(CHARGER).c

# Add USB-PD
ifeq ($(CONFIG_HAVE_USBPD),y)
CFLAGS += -DCONFIG_HAVE_USBPD=1
board-common-$(CONFIG_USBPD_TPS65987) += usbpd/tps65987.c
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
board-common-y += kbled/$(KBLED).c
endif

# Add scratch ROM
include $(SYSTEM76_COMMON_DIR)/scratch/scratch.mk

# Add scratch ROM for flash access
include $(SYSTEM76_COMMON_DIR)/flash/flash.mk
board-common-y += flash/wrapper.c

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
	cargo build --manifest-path ecflash/Cargo.toml --example isp --release
	sudo ecflash/target/release/examples/isp $<
