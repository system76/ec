# SPDX-License-Identifier: GPL-3.0-only

KEYMAP?=combo_iso_ansi
keyboard-y += keymap/$(KEYMAP).c

INCLUDE += $(KEYBOARD_DIR)/keyboard.mk
CFLAGS+=-I$(KEYBOARD_DIR)/include
