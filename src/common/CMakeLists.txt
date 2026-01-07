# SPDX-License-Identifier: GPL-3.0-only

target_sources(ec PRIVATE
    i2c.c
    keymap.c
    version.c
)

target_include_directories(ec PUBLIC include)
