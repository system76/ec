# SPDX-License-Identifier: GPL-3.0-only

target_sources(ec PRIVATE
    arch.c
    delay.c
    time.c
)

target_include_directories(ec PUBLIC include)

target_compile_definitions(ec PUBLIC __ARCH__=${ARCH})
