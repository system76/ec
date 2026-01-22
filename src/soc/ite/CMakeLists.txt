# SPDX-License-Identifier: GPL-3.0-only

target_sources(ec PRIVATE
    soc.c
    gpio.c
    i2c.c
    intc.c
    kbc.c
    pmc.c
    ps2.c
    signature.c
    wuc.c
)

if(CONFIG_BUS_ESPI)
    target_sources(ec PRIVATE espi.c)
endif()

target_include_directories(ec PUBLIC include)

set(CONFIG_CLOCK_FREQ_KHZ 9200)
target_compile_definitions(ec PUBLIC CONFIG_CLOCK_FREQ_KHZ=${CONFIG_CLOCK_FREQ_KHZ})

set(ARCH 8051 CACHE STRING "")

# 64 KiB is the max without banking.
set(CODE_SIZE 65536 CACHE STRING "")

if(CONFIG_SOC_ITE_IT8587E)
    target_compile_definitions(ec PUBLIC CONFIG_SOC_ITE_IT8587E=1)
    # SRAM is 4096 bytes, but SRAM at address 2048 is used for scratch ROM
    set(SRAM_SIZE 2048 CACHE STRING "")
elseif(CONFIG_SOC_ITE_IT5570E)
    target_compile_definitions(ec PUBLIC CONFIG_SOC_ITE_IT5570E=1)
    # SRAM is 6144 bytes, only 4096 bytes are mapped at address 0. Region at
    # 0x0E00-0x1000 is used for AP communication. So this is brought down to 2048,
    # which matches IT8587E limits
    set(SRAM_SIZE 2048 CACHE STRING "")
elseif(CONFIG_SOC_ITE_IT5571E)
    # The IT5571E is effectively the same as the IT5570E.
    target_compile_definitions(ec PUBLIC CONFIG_SOC_ITE_IT5571E=1)
    set(SRAM_SIZE 2048 CACHE STRING "")
else()
    message(FATAL_ERROR "unsupported SoC")
endif()

if(CONFIG_SOC_FLASH_SIZE_128K)
    set(CONFIG_SOC_FLASH_SIZE 131072 CACHE STRING "")
elseif(CONFIG_SOC_FLASH_SIZE_256K)
    set(CONFIG_SOC_FLASH_SIZE 262144 CACHE STRING "")
else()
    message(FATAL_ERROR "flash size not specified")
endif()

target_compile_definitions(ec PUBLIC CONFIG_SOC_FLASH_SIZE=${CONFIG_SOC_FLASH_SIZE})
