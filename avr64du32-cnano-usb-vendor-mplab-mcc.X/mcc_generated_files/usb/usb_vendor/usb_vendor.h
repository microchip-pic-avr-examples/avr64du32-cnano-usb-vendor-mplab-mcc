/**
 * USBVENDOR Vendor Header File
 * @file usb_vendor.h
 * @defgroup usb_vendor USB Vendor Class
 * @brief Contains the prototypes and data types for the vendor application drivers.
 * @version USB Device Stack Driver Version 1.0.0
 * @{
 */

/*
    (c) 2023 Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip software and any
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party
    license terms applicable to your use of third party software (including open source software) that
    may accompany Microchip software.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS
    FOR A PARTICULAR PURPOSE.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS
    SOFTWARE.
 */

#ifndef USB_VENDOR_H
#define	USB_VENDOR_H

#include "usb_protocol_headers.h"

/**
 * @ingroup usb_vendor
 * @brief Sets up interfaces for use with the Vendor class.
 * @param interfaceEnabledCallback - Callback to the interface enable function
 * @param vendorControlRequest - Callback to the control request function
 * @param interfaceDisabledCallback - Callback to the interface disable function
 * @return None.
 */
void USB_VendorClassInit(USB_SETUP_EVENT_CALLBACK_t interfaceEnabledCallback, USB_SETUP_PROCESS_CALLBACK_t vendorControlRequest, USB_EVENT_CALLBACK_t interfaceDisabledCallback);

/**
 * @}
 */

#endif	/* USB_VENDOR_H */