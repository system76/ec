/*
             LUFA Library
     Copyright (C) Dean Camera, 2018.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2018  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaims all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

#include "RawHID.h"

#define FLASH_SIZE_BYTES 32768UL
#define BOOTLOADER_SEC_SIZE_BYTES 4096UL

/* From http://www.fourwalledcubicle.com/files/LUFA/Doc/170418/html/_page__software_bootloader_start.html { */
uint32_t Boot_Key ATTR_NO_INIT;
#define MAGIC_BOOT_KEY            0xBADCAFE5
#define BOOTLOADER_START_ADDRESS  ((FLASH_SIZE_BYTES - BOOTLOADER_SEC_SIZE_BYTES) >> 1)
void Bootloader_Jump_Check(void) ATTR_INIT_SECTION(3);
void Bootloader_Jump_Check(void)
{
    // If the reset source was the bootloader and the key is correct, clear it and jump to the bootloader
    if ((MCUSR & (1 << WDRF)) && (Boot_Key == MAGIC_BOOT_KEY))
    {
        Boot_Key = 0;
        ((void (*)(void))BOOTLOADER_START_ADDRESS)();
    }
}

void Jump_To_Bootloader(void)
{
    // If USB is used, detach from the bus and reset it
    USB_Disable();
    // Disable all interrupts
    cli();
    // Wait two seconds for the USB detachment to register on the host
    Delay_MS(2000);
    // Set the bootloader key to the magic value and force a reset
    Boot_Key = MAGIC_BOOT_KEY;
    wdt_enable(WDTO_250MS);
    for (;;);
}
/* } */

/** Event handler for the USB_ConfigurationChanged event. This is fired when the host sets the current configuration
 *  of the USB device after enumeration, and configures the generic HID device endpoints.
 */
void EVENT_USB_Device_ConfigurationChanged(void) {
	bool ConfigSuccess = true;

	/* Setup HID Report Endpoints */
	ConfigSuccess &= Endpoint_ConfigureEndpoint(RAW_IN_EPADDR, EP_TYPE_INTERRUPT, RAW_EPSIZE, 1);
	ConfigSuccess &= Endpoint_ConfigureEndpoint(RAW_OUT_EPADDR, EP_TYPE_INTERRUPT, RAW_EPSIZE, 1);

	/* Indicate endpoint configuration success or failure */
}

void raw_hid_send(uint8_t *data, uint8_t length) {
    // TODO: implement variable size packet
    if (length != RAW_EPSIZE) {
        return;
    }

    if (USB_DeviceState != DEVICE_STATE_Configured) {
        return;
    }

    // TODO: decide if we allow calls to raw_hid_send() in the middle
    // of other endpoint usage.
    uint8_t ep = Endpoint_GetCurrentEndpoint();

    Endpoint_SelectEndpoint(RAW_IN_EPNUM);

    // Check to see if the host is ready to accept another packet
    if (Endpoint_IsINReady()) {
        // Write data
        Endpoint_Write_Stream_LE(data, RAW_EPSIZE, NULL);
        // Finalize the stream transfer to send the last packet
        Endpoint_ClearIN();
    }

    Endpoint_SelectEndpoint(ep);
}

static void raw_hid_task(void) {
    // Create a temporary buffer to hold the read in data from the host
    uint8_t data[RAW_EPSIZE];
    bool    data_read = false;

    // Device must be connected and configured for the task to run
    if (USB_DeviceState != DEVICE_STATE_Configured) return;

    Endpoint_SelectEndpoint(RAW_OUT_EPNUM);

    // Check to see if a packet has been sent from the host
    if (Endpoint_IsOUTReceived()) {
        // Check to see if the packet contains data
        if (Endpoint_IsReadWriteAllowed()) {
            /* Read data */
            Endpoint_Read_Stream_LE(data, sizeof(data), NULL);
            data_read = true;
        }

        // Finalize the stream transfer to receive the last packet
        Endpoint_ClearOUT();

        if (data_read) {
            raw_hid_receive(data, sizeof(data));
        }
    }
}

int main(void) {
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable clock division */
	clock_prescale_set(clock_div_1);

    raw_hid_init();

	/* Hardware Initialization */
	USB_Init();

	GlobalInterruptEnable();

	for (;;)
	{
		raw_hid_task();
		USB_USBTask();
	}
}
