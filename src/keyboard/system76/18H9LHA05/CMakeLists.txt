# SPDX-License-Identifier: GPL-3.0-only

if(NOT DEFINED KEYMAP)
    set(KEYMAP default)
endif()

target_sources(ec PRIVATE keymap/${KEYMAP}.c)

target_include_directories(ec PUBLIC include)
