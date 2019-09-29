#include <stdio.h>

#include "include/stdio.h"

void init(void) {
    stdio_init(0, 9600);
}

void main(void) {
    init();

    printf("Hello from System76 EC for the Arduino Mega 2560!\n");
    for (;;) {}
}
