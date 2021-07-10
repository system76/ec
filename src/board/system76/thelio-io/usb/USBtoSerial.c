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

#include "USBtoSerial.h"

#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/power.h>

#include "Descriptors.h"
#include <LUFA/Drivers/USB/USB.h>
#include <LUFA/Platform/Platform.h>

/** LUFA CDC Class driver interface configuration and state information. This structure is
 *  passed to all CDC Class driver functions, so that multiple instances of the same class
 *  within a device can be differentiated from one another.
 */
USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface =
	{
		.Config =
			{
				.ControlInterfaceNumber         = INTERFACE_ID_CDC_CCI,
				.DataINEndpoint                 =
					{
						.Address                = CDC_TX_EPADDR,
						.Size                   = CDC_TXRX_EPSIZE,
						.Banks                  = 1,
					},
				.DataOUTEndpoint                =
					{
						.Address                = CDC_RX_EPADDR,
						.Size                   = CDC_TXRX_EPSIZE,
						.Banks                  = 1,
					},
				.NotificationEndpoint           =
					{
						.Address                = CDC_NOTIFICATION_EPADDR,
						.Size                   = CDC_NOTIFICATION_EPSIZE,
						.Banks                  = 1,
					},
			},
	};

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

void USBtoSerial_Bootloader(void) {
    Jump_To_Bootloader();
}

void USBtoSerial_Init(void) {
#if (ARCH == ARCH_AVR8)
	/* Disable watchdog if enabled by bootloader/fuses */
	MCUSR &= ~(1 << WDRF);
	wdt_disable();

	/* Disable clock division */
	clock_prescale_set(clock_div_1);
#endif

	GlobalInterruptEnable();
	USB_Init();
}

int16_t USBtoSerial_Read(void) {
	return CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
}

uint8_t USBtoSerial_Write(uint8_t byte) {
	return CDC_Device_SendByte(&VirtualSerial_CDC_Interface, byte);
}

void USBtoSerial_Task(void) {
	CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
	USB_USBTask();
}

void USBtoSerial_Destroy(void) {
	USB_Disable();
}

void EVENT_USB_Device_ConfigurationChanged(void) {
	CDC_Device_ConfigureEndpoints(&VirtualSerial_CDC_Interface);
}

void EVENT_USB_Device_ControlRequest(void) {
	CDC_Device_ProcessControlRequest(&VirtualSerial_CDC_Interface);
}

/** Event handler for the CDC Class driver Line Encoding Changed event.
 *
 *  \param[in] CDCInterfaceInfo  Pointer to the CDC class interface configuration structure being referenced
 */
void EVENT_CDC_Device_LineEncodingChanged(USB_ClassInfo_CDC_Device_t* const CDCInterfaceInfo) {}
