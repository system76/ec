#include <board/acpi.h>
#include <common/debug.h>

uint8_t acpi_read(uint8_t addr) {
    uint8_t data = 0;
    DEBUG("acpi_read %02X = %02X\n", addr, data);
    switch (addr) {
        //TODO
    }
    return data;
}

void acpi_write(uint8_t addr, uint8_t data) {
    DEBUG("acpi_write %02X = %02X\n", addr, data);
    switch (addr) {
        //TODO
    }
}
