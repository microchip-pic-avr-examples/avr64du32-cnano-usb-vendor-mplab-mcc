/**
 * USB_DEVICE_STACK Generated Driver File
 * 
 * @file usb_descriptors.c
 * 
 * @ingroup usb_device_stack
 * 
 * @brief Driver implementation file for example application descriptors.
 *
 * @version USB_DEVICE_STACK Driver Version 1.0.0
*/
/*
� [2024] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip 
    software and any derivatives exclusively with Microchip products. 
    You are responsible for complying with 3rd party license terms  
    applicable to your use of 3rd party software (including open source  
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.? 
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS 
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,  
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT 
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY 
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF 
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE 
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S 
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT 
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR 
    THIS SOFTWARE.
*/

#include <string.h>
#include "usb_descriptors.h"
#include <usb_protocol_headers.h>


static USB_DEVICE_DESCRIPTOR_t deviceDescriptor = {
    .header = {
        .bLength = sizeof(USB_DEVICE_DESCRIPTOR_t),
        .bDescriptorType = USB_DESCRIPTOR_TYPE_DEVICE,
    },
    .bcdUSB = 0x200,            // USB 2.0
    .bDeviceClass = USB_VENDOR_DEVICE_CLASS,        // Not defined on device level
    .bDeviceSubClass = 0x00,            // Not defined in Device Descriptor level
    .bDeviceProtocol = 0x00,            // Not defined in Device Descriptor level
    .bMaxPacketSize0 = USB_EP0_SIZE,    // EP0 size
    .idVendor = 0x04D8,            // MCHP VID
    .idProduct = 0x0B0A,          // PID 0x0010-0x002F reserved for testing/non-public demos
    .bcdDevice = 0x0110,   // 01.1.0,
    .iManufacturer = 0x01,              // String index 1
    .iProduct = 0x02,                   // String index 2
    .iSerialNumber = 0x03,              // String index 3
    .bNumConfigurations = 0x01          // Number of configurations 
};

static USB_APPLICATION_CONFIGURATION_t configurationDescriptor = {
    .Config1 =
    {
        .Configuration =
        {
            .header = 
            {
                .bLength = sizeof (USB_CONFIGURATION_DESCRIPTOR_t),
                .bDescriptorType = (uint8_t)USB_DESCRIPTOR_TYPE_CONFIGURATION,
            },
            .wTotalLength = sizeof (USB_APPLICATION_CONFIGURATION1_t),
            .bNumInterfaces = USB_INTERFACE_NUM,
            .bConfigurationValue = 1u,
            .iConfiguration = 0u,
            .bmAttributes = USB_CONFIG_ATTR_MUST_SET | USB_CONFIG_ATTR_BUS_POWERED,
            .bMaxPower = USB_CONFIG_MAX_POWER(2),
        },
        .Interface0Alternate0 =
        {
            .header =
            {
                .bLength = sizeof (USB_INTERFACE_DESCRIPTOR_t),
                .bDescriptorType = USB_DESCRIPTOR_TYPE_INTERFACE,
            },
            .bInterfaceNumber = 0U,
            .bAlternateSetting = 0U,
            .bNumEndpoints = 0U,
            .bInterfaceClass = 0xFF, // Vendor
            .bInterfaceSubClass = 0xFF,
            .bInterfaceProtocol = 0xFF,
            .iInterface = 0U,
        },        
        .Interface0Alternate1 =
        {
            .header =
            {
                .bLength = sizeof (USB_INTERFACE_DESCRIPTOR_t),
                .bDescriptorType = USB_DESCRIPTOR_TYPE_INTERFACE,
            },
            .bInterfaceNumber = 0U,
            .bAlternateSetting = 1U,
            .bNumEndpoints = 4U,
            .bInterfaceClass = 0xFF, // Vendor
            .bInterfaceSubClass = 0xFF,
            .bInterfaceProtocol = 0xFF,
            .iInterface = 0U,
        },        
        .Interface0Alternate1_Endpoint1IN =
        {
            .header =
            {
                .bLength = sizeof (USB_ENDPOINT_DESCRIPTOR_t),
                .bDescriptorType = USB_DESCRIPTOR_TYPE_ENDPOINT,
            },
            .bEndpointAddress =
            {
                .direction = USB_EP_DIR_IN,
                .address = INTERFACE0ALTERNATE1_INTERRUPT_EP1_IN,
            },
            .bmAttributes =
            {
                .type = INTERRUPT,
                .synchronisation = 0U, // None
                .usage = 0U, // None
            },
            .wMaxPacketSize = INTERFACE0ALTERNATE1_INTERRUPT_EP1_IN_SIZE,
            .bInterval = 1U,
        },
        .Interface0Alternate1_Endpoint1OUT =
        {
            .header =
            {
                .bLength = sizeof (USB_ENDPOINT_DESCRIPTOR_t),
                .bDescriptorType = USB_DESCRIPTOR_TYPE_ENDPOINT,
            },
            .bEndpointAddress =
            {
                .direction = USB_EP_DIR_OUT,
                .address = INTERFACE0ALTERNATE1_INTERRUPT_EP1_OUT,
            },
            .bmAttributes =
            {
                .type = INTERRUPT,
                .synchronisation = 0U, // None
                .usage = 0U, // None
            },
            .wMaxPacketSize = INTERFACE0ALTERNATE1_INTERRUPT_EP1_OUT_SIZE,
            .bInterval = 1U,
        },
        .Interface0Alternate1_Endpoint2IN =
        {
            .header =
            {
                .bLength = sizeof (USB_ENDPOINT_DESCRIPTOR_t),
                .bDescriptorType = USB_DESCRIPTOR_TYPE_ENDPOINT,
            },
            .bEndpointAddress =
            {
                .direction = USB_EP_DIR_IN,
                .address = INTERFACE0ALTERNATE1_BULK_EP2_IN,
            },
            .bmAttributes =
            {
                .type = BULK,
                .synchronisation = 0U, // None
                .usage = 0U, // None
            },
            .wMaxPacketSize = INTERFACE0ALTERNATE1_BULK_EP2_IN_SIZE,
            .bInterval = 0U,
        },
        .Interface0Alternate1_Endpoint2OUT =
        {
            .header =
            {
                .bLength = sizeof (USB_ENDPOINT_DESCRIPTOR_t),
                .bDescriptorType = USB_DESCRIPTOR_TYPE_ENDPOINT,
            },
            .bEndpointAddress =
            {
                .direction = USB_EP_DIR_OUT,
                .address = INTERFACE0ALTERNATE1_BULK_EP2_OUT,
            },
            .bmAttributes =
            {
                .type = BULK,
                .synchronisation = 0U, // None
                .usage = 0U, // None
            },
            .wMaxPacketSize = INTERFACE0ALTERNATE1_BULK_EP2_OUT_SIZE,
            .bInterval = 0U,
        },
        .Interface0Alternate2 =
        {
            .header =
            {
                .bLength = sizeof (USB_INTERFACE_DESCRIPTOR_t),
                .bDescriptorType = USB_DESCRIPTOR_TYPE_INTERFACE,
            },
            .bInterfaceNumber = 0U,
            .bAlternateSetting = 2U,
            .bNumEndpoints = 2U,
            .bInterfaceClass = 0xFF, // Vendor
            .bInterfaceSubClass = 0xFF,
            .bInterfaceProtocol = 0xFF,
            .iInterface = 0U,
        },        
        .Interface0Alternate2_Endpoint3IN =
        {
            .header =
            {
                .bLength = sizeof (USB_ENDPOINT_DESCRIPTOR_t),
                .bDescriptorType = USB_DESCRIPTOR_TYPE_ENDPOINT,
            },
            .bEndpointAddress =
            {
                .direction = USB_EP_DIR_IN,
                .address = INTERFACE0ALTERNATE2_ISOCHRONOUS_EP3_IN,
            },
            .bmAttributes =
            {
                .type = ISOCHRONOUS,
                .synchronisation = 1U, // Asynchronous
                .usage = 0U, // Data
            },
            .wMaxPacketSize = INTERFACE0ALTERNATE2_ISOCHRONOUS_EP3_IN_SIZE,
            .bInterval = 1U,
        },
        .Interface0Alternate2_Endpoint3OUT =
        {
            .header =
            {
                .bLength = sizeof (USB_ENDPOINT_DESCRIPTOR_t),
                .bDescriptorType = USB_DESCRIPTOR_TYPE_ENDPOINT,
            },
            .bEndpointAddress =
            {
                .direction = USB_EP_DIR_OUT,
                .address = INTERFACE0ALTERNATE2_ISOCHRONOUS_EP3_OUT,
            },
            .bmAttributes =
            {
                .type = ISOCHRONOUS,
                .synchronisation = 1U, // Asynchronous
                .usage = 0U, // Data
            },
            .wMaxPacketSize = INTERFACE0ALTERNATE2_ISOCHRONOUS_EP3_OUT_SIZE,
            .bInterval = 1U,
        },
        .Interface0Alternate3 =
        {
            .header =
            {
                .bLength = sizeof (USB_INTERFACE_DESCRIPTOR_t),
                .bDescriptorType = USB_DESCRIPTOR_TYPE_INTERFACE,
            },
            .bInterfaceNumber = 0U,
            .bAlternateSetting = 3U,
            .bNumEndpoints = 1U,
            .bInterfaceClass = 0xFF, // Vendor
            .bInterfaceSubClass = 0xFF,
            .bInterfaceProtocol = 0xFF,
            .iInterface = 0U,
        },        
        .Interface0Alternate3_Endpoint4OUT =
        {
            .header =
            {
                .bLength = sizeof (USB_ENDPOINT_DESCRIPTOR_t),
                .bDescriptorType = USB_DESCRIPTOR_TYPE_ENDPOINT,
            },
            .bEndpointAddress =
            {
                .direction = USB_EP_DIR_OUT,
                .address = INTERFACE0ALTERNATE3_ISOCHRONOUS_EP4_OUT,
            },
            .bmAttributes =
            {
                .type = ISOCHRONOUS,
                .synchronisation = 1U, // Asynchronous
                .usage = 0U, // Data
            },
            .wMaxPacketSize = INTERFACE0ALTERNATE3_ISOCHRONOUS_EP4_OUT_SIZE,
            .bInterval = 1U,
        },
        .Interface0Alternate4 =
        {
            .header =
            {
                .bLength = sizeof (USB_INTERFACE_DESCRIPTOR_t),
                .bDescriptorType = USB_DESCRIPTOR_TYPE_INTERFACE,
            },
            .bInterfaceNumber = 0U,
            .bAlternateSetting = 4U,
            .bNumEndpoints = 1U,
            .bInterfaceClass = 0xFF, // Vendor
            .bInterfaceSubClass = 0xFF,
            .bInterfaceProtocol = 0xFF,
            .iInterface = 0U,
        },        
        .Interface0Alternate4_Endpoint4IN =
        {
            .header =
            {
                .bLength = sizeof (USB_ENDPOINT_DESCRIPTOR_t),
                .bDescriptorType = USB_DESCRIPTOR_TYPE_ENDPOINT,
            },
            .bEndpointAddress =
            {
                .direction = USB_EP_DIR_IN,
                .address = INTERFACE0ALTERNATE4_ISOCHRONOUS_EP4_IN,
            },
            .bmAttributes =
            {
                .type = ISOCHRONOUS,
                .synchronisation = 1U, // Asynchronous
                .usage = 0U, // Data
            },
            .wMaxPacketSize = INTERFACE0ALTERNATE4_ISOCHRONOUS_EP4_IN_SIZE,
            .bInterval = 1U,
        },
    },
};

static USB_STRING_LANG_ID_DESCRIPTOR_t langIDDescriptor  = {
    .header =
    {
        .bLength = sizeof (USB_STRING_LANG_ID_DESCRIPTOR_t),
        .bDescriptorType = USB_DESCRIPTOR_TYPE_STRING,
    },
    .id_array =
    {LANG_EN_US},
};

static USB_APPLICATION_STRING_DESCRIPTORS_t stringDescriptors = {
    .manufacturer_header =
    {
        .bLength = sizeof (stringDescriptors.manufacturer) + sizeof (USB_DESCRIPTOR_HEADER_t),
        .bDescriptorType = USB_DESCRIPTOR_TYPE_STRING,
    },
    .manufacturer = STRING_MANUFACTURER,
    .product_header =
    {
        .bLength = sizeof (stringDescriptors.product) + sizeof (USB_DESCRIPTOR_HEADER_t),
        .bDescriptorType = USB_DESCRIPTOR_TYPE_STRING,
    },
    .product = STRING_PRODUCT,
    .serial_header =
    {
        .bLength = sizeof (stringDescriptors.serial) + sizeof (USB_DESCRIPTOR_HEADER_t),
        .bDescriptorType = USB_DESCRIPTOR_TYPE_STRING,
    },
    .serial = STRING_SERIAL,
};

USB_DESCRIPTOR_POINTERS_t descriptorPointers = {
    .devicePtr = (USB_DEVICE_DESCRIPTOR_t *) & deviceDescriptor,
    .configurationsPtr = (USB_CONFIGURATION_DESCRIPTOR_t *) & configurationDescriptor,
    .deviceBOSptr = NULL,
    .langIDptr = &langIDDescriptor,
    .stringPtrs =
    {
        &stringDescriptors.manufacturer_header,
    },
};

/**
 End of File
*/
