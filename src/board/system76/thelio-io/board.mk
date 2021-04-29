# SPDX-License-Identifier: GPL-3.0-only

EC=atmega32u4

# Add USB support
USB_DIR=$(BOARD_DIR)/usb
USB_SRC=\
	$(USB_DIR)/makefile \
	$(USB_DIR)/*.c \
	$(USB_DIR)/*.h \
	$(USB_DIR)/Config/*.h
USB_OBJ=$(USB_DIR)/libUSBtoSerial.a
$(USB_OBJ): $(USB_SRC)
	make -C $(USB_DIR) lib
LATE_OBJ+=$(USB_OBJ)

bootloader:
	echo 1 | sudo tee /sys/bus/usb/drivers/system76-io/*.1/bootloader

dfu: $(BUILD)/ec.ihx
	sudo dfu-programmer $(EC) flash $<
	sudo dfu-programmer $(EC) reset

# Build DFU bootloader
BOOT_DIR=$(BOARD_DIR)/bootloader
BOOT_SRC=\
	$(BOOT_DIR)/makefile \
	$(BOOT_DIR)/*.c \
	$(BOOT_DIR)/*.h \
	$(BOOT_DIR)/Config/*.h
BOOT_HEX=$(BOOT_DIR)/BootloaderDFU.hex
$(BOOT_HEX): $(BOOT_SRC)
	make -C $(BOOT_DIR) hex

icsp: $(BOOT_HEX)
	sudo avrdude -v -v \
		-c usbasp \
		-p $(EC) \
		-U flash:w:$<:i\
		-U lfuse:w:0xFF:m \
		-U hfuse:w:0xD9:m \
		-U efuse:w:0xCB:m
