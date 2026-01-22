# SPDX-License-Identifier: GPL-3.0-only
# SPDX-FileCopyrightText: 2026 System76, Inc.

# cmake --toolchain cmake/toolchain-sdcc.cmake -B build

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
list(PREPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Target version available on Ubuntu 24.04.
cmake_minimum_required(VERSION 3.28)
project(ec
    LANGUAGES C ASM
)

add_executable(ec)

# Project uses git commit details as the version.
find_package(Git QUIET REQUIRED)
execute_process(COMMAND
    ${GIT_EXECUTABLE} show --format=%cs --no-patch --no-show-signature
    OUTPUT_VARIABLE GIT_DATE
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(COMMAND
    ${GIT_EXECUTABLE} describe --abbrev=7 --always --dirty
    OUTPUT_VARIABLE GIT_REV
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(VERSION ${GIT_DATE}_${GIT_REV})
set(BOARD system76/darp10)

target_compile_definitions(ec PUBLIC __FIRMWARE_VERSION__=${VERSION})
target_compile_definitions(ec PUBLIC __BOARD__=${BOARD})

add_subdirectory(src/common)
add_subdirectory(src/board/${BOARD})

if(NOT DEFINED KEYBOARD)
    message(FATAL_ERROR "KEYBOARD is not set by the board")
endif()
add_subdirectory(src/keyboard/system76/${KEYBOARD})

add_subdirectory(src/soc/${EC})
add_subdirectory(src/arch/${ARCH})
add_subdirectory(src/app/main)

add_custom_command(
    TARGET ec
    POST_BUILD
    COMMENT "Checking HOME segment starts at 0"
    COMMAND sh ./scripts/check-home-segment.sh
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

# SDCC only generates Intel HEX, but tools expect the binary.
add_custom_command(
    TARGET ec
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -I ihex -O binary
        --gap-fill=0xFF --pad-to=${CONFIG_SOC_FLASH_SIZE}
        "$<TARGET_FILE:ec>" "ec.rom"
    BYPRODUCTS "ec.rom"
)

add_custom_command(
    TARGET ec
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E cmake_echo_color --green --bold "Built ${VERSION} for ${BOARD}"
)

add_custom_target(lint
    COMMAND bash ./scripts/lint/lint.sh
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

add_custom_target(docs
    COMMAND mdbook build docs/
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

# Set external programmer
#PROGRAMMER=$(wildcard /dev/serial/by-id/usb-Arduino*)
#
#console_internal:
#    cargo build --manifest-path tools/system76_ectool/Cargo.toml --release
#    sudo tools/system76_ectool/target/release/system76_ectool console
#
#console_external:
#    sudo test -c "$(PROGRAMMER)"
#    sleep 1 && echo C | sudo tee "$(PROGRAMMER)" &
#    sudo tio -b 1000000 -m INLCRNL -t "$(PROGRAMMER)"
#
#console_external_forced:
#    sudo test -c "$(PROGRAMMER)"
#    sleep 1 && echo F | sudo tee "$(PROGRAMMER)" &
#    sudo tio -b 1000000 -m INLCRNL -t "$(PROGRAMMER)"
#
#flash_internal: $(BUILD)/ec.rom
#    cargo build --manifest-path tools/system76_ectool/Cargo.toml --release
#    sudo tools/system76_ectool/target/release/system76_ectool flash $<
#
#flash_external: $(BUILD)/ec.rom
#    cargo build --manifest-path tools/ecflash/Cargo.toml --example isp --release
#    sudo tools/ecflash/target/release/examples/isp $<
