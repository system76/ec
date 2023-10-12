# SPDX-License-Identifier: GPL-3.0-only

CC = sdcc -mmcs51 -MMD --model-large --code-size $(CODE_SIZE) --xram-size $(SRAM_SIZE) --Werror

AS = sdas8051
ASFLAGS = -plosgff

# XXX: SDCC "requires" main() to be in the first item passed to the linker.
# Ref: SDCC Manual 4.2.8; Section 3.2.3 Projects with Multiple Source Files
MAIN_SRC = $(filter %/main.c, $(SRC))
MAIN_OBJ = $(sort $(patsubst src/%.c, $(BUILD)/%.rel, $(MAIN_SRC)))

# NOTE: sdas *must* be used for assembly files as sdcc cannot compile them
# itself. They must use an extension of `.asm` so they can be filtered here.

ASM_SRC = $(filter %.asm, $(SRC))
ASM_OBJ = $(sort $(patsubst src/%.asm, $(BUILD)/%.rel, $(ASM_SRC)))

C_SRC = $(filter %.c, $(SRC))
C_OBJ = $(sort $(patsubst src/%.c, $(BUILD)/%.rel, $(C_SRC)))

OBJ = $(MAIN_OBJ) $(sort $(ASM_OBJ) $(C_OBJ))

# Run EC rom in simulator
sim: $(BUILD)/ec.rom
	cargo run \
		--release \
		--manifest-path ecsim/Cargo.toml \
		--no-default-features \
		-- $<

# Convert from Intel Hex file to binary file
$(BUILD)/ec.rom: $(BUILD)/ec.ihx
	@echo "  OBJCOPY   $(subst $(BUILD)/,,$@)"
	mkdir -p $(@D)
	objcopy -I ihex -O binary --gap-fill=0xFF --pad-to=$(CONFIG_EC_FLASH_SIZE) $< $@

# Link object files into Intel Hex file
$(BUILD)/ec.ihx: $(OBJ)
	@echo "  LINK      $(subst $(BUILD)/,,$@)"
	mkdir -p $(@D)
	$(CC) $(LDFLAGS) -o $@ $^

# Compile ASM files into object files
$(ASM_OBJ): $(BUILD)/%.rel: src/%.asm
	@echo "  AS        $(subst $(BUILD)/,,$@)"
	mkdir -p $(@D)
	$(AS) $(ASFLAGS) $@ $<

# Compile C files into object files
$(C_OBJ): $(BUILD)/%.rel: src/%.c $(INCLUDE)
	@echo "  CC        $(subst $(BUILD)/,,$@)"
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ -c $<

# Add dependency rules
DEP = $(OBJ:%.rel=%.d)
-include $(DEP)
