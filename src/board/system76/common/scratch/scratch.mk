# SPDX-License-Identifier: GPL-3.0-only

scratch-y += main.c
scratch-y += stdio.c
scratch-y += ../smfi.c

# Set scratch ROM parameters
SCRATCH_OFFSET=1024
SCRATCH_SIZE=1024
CFLAGS+=-DSCRATCH_OFFSET=$(SCRATCH_OFFSET) -DSCRATCH_SIZE=$(SCRATCH_SIZE)

# Copy parameters to use when compiling scratch ROM
SCRATCH_INCLUDE=$(INCLUDE)
SCRATCH_CFLAGS=$(CFLAGS)

# Include scratch source
SCRATCH_DIR=$(SYSTEM76_COMMON_DIR)/scratch
SCRATCH_INCLUDE += $(SCRATCH_DIR)/scratch.mk
SCRATCH_CFLAGS+=-I$(SCRATCH_DIR)/include -D__SCRATCH__
SCRATCH_SRC += $(foreach src, $(scratch-y), $(SCRATCH_DIR)/$(src))

SCRATCH_BUILD=$(BUILD)/scratch
SCRATCH_OBJ=$(sort $(patsubst src/%.c,$(SCRATCH_BUILD)/%.rel,$(SCRATCH_SRC)))
SCRATCH_CC=\
	$(SDCC) \
	-mmcs51 \
	-MMD \
	--model-small \
	--code-loc $(SCRATCH_OFFSET) \
	--code-size $(SCRATCH_SIZE) \
	--Werror

# Convert from binary file to C header
$(BUILD)/include/scratch.h: $(SCRATCH_BUILD)/scratch.rom
	@echo "  XXD       $(subst $(BUILD)/,,$@)"
	mkdir -p $(@D)
	xxd -include -s $(SCRATCH_OFFSET) < $< > $@

# Convert from Intel Hex file to binary file
$(SCRATCH_BUILD)/scratch.rom: $(SCRATCH_BUILD)/scratch.ihx
	@echo "  MAKEBIN   $(subst $(BUILD)/,,$@)"
	mkdir -p $(@D)
	makebin -p $< $@

# Link object files into Intel Hex file
$(SCRATCH_BUILD)/scratch.ihx: $(SCRATCH_OBJ)
	@echo "  LINK      $(subst $(BUILD)/,,$@)"
	mkdir -p $(@D)
	$(SCRATCH_CC) -o $@ $^

# Compile C files into object files
$(SCRATCH_OBJ): $(SCRATCH_BUILD)/%.rel: src/%.c $(SCRATCH_INCLUDE)
	@echo "  CC        $(subst $(BUILD)/,,$@)"
	mkdir -p $(@D)
	$(SCRATCH_CC) $(SCRATCH_CFLAGS) -o $@ -c $<

# Include scratch header in main firmware
CFLAGS+=-I$(BUILD)/include
INCLUDE+=$(BUILD)/include/scratch.h
