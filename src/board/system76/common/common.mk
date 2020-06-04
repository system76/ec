# Include system76 common source
SYSTEM76_COMMON_DIR=src/board/system76/common
SRC+=$(wildcard $(SYSTEM76_COMMON_DIR)/*.c)
INCLUDE+=$(wildcard $(SYSTEM76_COMMON_DIR)/include/common/*.h) $(SYSTEM76_COMMON_DIR)/common.mk
CFLAGS+=-I$(SYSTEM76_COMMON_DIR)/include

# Add scratch ROM
include $(SYSTEM76_COMMON_DIR)/scratch/scratch.mk

console_internal:
	cargo build --manifest-path tool/Cargo.toml --release
	sudo tool/target/release/system76_ectool console

console_external:
	sleep 1 && echo C | sudo tee /dev/ttyACM* &
	sudo tio -b 1000000 -m INLCRNL -t /dev/ttyACM*

flash_internal: $(BUILD)/ec.rom
	cargo build --manifest-path tool/Cargo.toml --release
	sudo tool/target/release/system76_ectool flash $<

flash_external: $(BUILD)/ec.rom
	cargo build --manifest-path ecflash/Cargo.toml --example isp --release
	sudo ecflash/target/release/examples/isp $<
