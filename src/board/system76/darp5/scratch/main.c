#include <board/smfi.h>

// Main program while running in scratch ROM
void main(void) {
	for (;;) {
        smfi_event();
    }
}
