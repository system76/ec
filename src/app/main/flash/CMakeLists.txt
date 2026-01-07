# SPDX-License-Identifier: GPL-3.0-only

# Flash SROM app
add_executable(flash
    main.c
)

target_include_directories(flash PUBLIC include ../include ../../../common/include ../../../soc/${EC}/include)
target_compile_definitions(flash PUBLIC __FLASH__)
target_compile_definitions(flash PUBLIC CONFIG_SOC_FLASH_SIZE=${CONFIG_SOC_FLASH_SIZE})

# Set flash ROM parameters
set(FLASH_OFFSET 2048)
set(FLASH_SIZE 1024)

target_compile_options(flash PRIVATE
    -mmcs51
    --model-large
    --opt-code-size
    --acall-ajmp
    "--code-loc ${FLASH_OFFSET}"
    "--code-size ${FLASH_SIZE}"
    --Werror
)

target_link_options(flash PRIVATE
    -mmcs51
    --model-large
    --opt-code-size
    --acall-ajmp
    "--code-loc ${FLASH_OFFSET}"
    "--code-size ${FLASH_SIZE}"
    --Werror
)

# Convert from Intel Hex file to binary file
add_custom_command(
    TARGET flash
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -I ihex -O binary
        "$<TARGET_FILE:flash>" "flash.rom"
    BYPRODUCTS "flash.rom"
)

# Convert from binary file to C header
add_custom_command(
    TARGET flash
    POST_BUILD
    COMMENT "Generating include header"
    COMMAND mkdir -p "${CMAKE_CURRENT_BINARY_DIR}/include"
    COMMAND xxd -include < "${CMAKE_CURRENT_BINARY_DIR}/flash.rom" > "${CMAKE_CURRENT_BINARY_DIR}/include/flash.h"
)

# Add dependency to main app
target_compile_definitions(ec PUBLIC
    FLASH_OFFSET=${FLASH_OFFSET}
    FLASH_SIZE=${FLASH_SIZE}
)
target_include_directories(ec PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/include)
target_link_options(ec PUBLIC "-Wl -g_flash_entry=${FLASH_OFFSET}")
add_dependencies(ec flash)
