# SPDX-License-Identifier: GPL-3.0-only

# Scratch SROM app
add_executable(scratch
    ../smfi.c
    main.c
    stdio.c
)

target_include_directories(scratch PUBLIC include ../include ../../../common/include ../../../soc/${EC}/include)
target_compile_definitions(scratch PRIVATE __SCRATCH__)

# Set scratch ROM parameters
set(SCRATCH_OFFSET 1024)
set(SCRATCH_SIZE 1024)

target_compile_options(scratch PRIVATE
    -mmcs51
    --model-small
    --opt-code-size
    "--code-loc ${SCRATCH_OFFSET}"
    "--code-size ${SCRATCH_SIZE}"
    --Werror
)

target_link_options(scratch PRIVATE
    -mmcs51
    --model-small
    --opt-code-size
    "--code-loc ${SCRATCH_OFFSET}"
    "--code-size ${SCRATCH_SIZE}"
    --Werror
)

# Convert from Intel Hex file to binary file
add_custom_command(
    TARGET scratch
    POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -I ihex -O binary
        "$<TARGET_FILE:scratch>" "scratch.rom"
    BYPRODUCTS "scratch.rom"
)

# Convert from binary file to C header
add_custom_command(
    TARGET scratch
    POST_BUILD
    COMMENT "Generating include header"
    COMMAND mkdir -p "${CMAKE_CURRENT_BINARY_DIR}/include"
    COMMAND xxd -include < "${CMAKE_CURRENT_BINARY_DIR}/scratch.rom" > "${CMAKE_CURRENT_BINARY_DIR}/include/scratch.h"
)

# Add dependency to main app
target_compile_definitions(ec PUBLIC
    SCRATCH_OFFSET=${SCRATCH_OFFSET}
    SCRATCH_SIZE=${SCRATCH_SIZE}
)
target_include_directories(ec PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/include)
add_dependencies(ec scratch)
