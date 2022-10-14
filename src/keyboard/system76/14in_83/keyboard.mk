# SPDX-License-Identifier: GPL-3.0-only

KEYMAP?=default
keyboard-y += keymap/$(KEYMAP).c

INCLUDE+=$(wildcard $(KEYBOARD_DIR)/include/board/*.h) $(KEYBOARD_DIR)/keyboard.mk
CFLAGS+=-I$(KEYBOARD_DIR)/include
