 /*
 * MAIN Generated Driver File
 * 
 * @file main.c
 * 
 * @defgroup main MAIN
 * 
 * @brief Transfers data with USB Vendor Class
 *
 * @version MAIN Driver Version 1.0.0
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

#include "mcc_generated_files/system/system.h"
#include "usb_core.h"
#include <usb_vendor.h>

// USB Interface Number used by all Interfaces
#define INTERFACE_ENDPOINTS 0

// USB Alternate Interface Settings for the different transfer types
#define INTERFACE_ALTERNATE_CONTROL 0
#define INTERFACE_ALTERNATE_BULKINT 1
#define INTERFACE_ALTERNATE_ISO1 2
#define INTERFACE_ALTERNATE_ISO1023_OUT 3
#define INTERFACE_ALTERNATE_ISO1023_IN 4

// Vendor defines
#define VENDOR_DEFAULT_TRANSFER 4096ul
#define VENDOR_ISO_TRANSFER     (((VENDOR_DEFAULT_TRANSFER) / INTERFACE0ALTERNATE4_ISOCHRONOUS_EP4_IN_SIZE) * INTERFACE0ALTERNATE4_ISOCHRONOUS_EP4_IN_SIZE) // taking 1023 into account
#define REQUEST_VENDOR_TEST_CONTROL 0
#define REQUEST_VENDOR_CONFIGURE_BULKINT_ZLP 1
#define REQUEST_VENDOR_CONFIGURE_BULKINT_SIZE 2

// Timer defines
#define CONSECUTIVE_EQUAL_PIT_INTERRUPTS 5

// USB connection variables
volatile RETURN_CODE_t status = SUCCESS;
volatile bool vbusFlag = false;
volatile bool prevVbusState = false;

// Bulk transfer variables
uint8_t vendorBuffer[VENDOR_DEFAULT_TRANSFER] __attribute__((aligned(2))) = { 0 };
volatile uint16_t bulkintTransferSize = VENDOR_DEFAULT_TRANSFER;
volatile bool bulkintUseZLP = false;

// USB connection functions
void RTC_GetUSBState(void);
void CheckUSBConnection(void);

// Data transfer Functions
void InterfaceEnabledCallback(USB_SETUP_REQUEST_t *setupRequestPtr);
RETURN_CODE_t VendorControlRequest(USB_SETUP_REQUEST_t *setupRequestPtr);
void VendorDataSent(USB_PIPE_t pipe, USB_TRANSFER_STATUS_t status, uint16_t bytesTransferred);
void VendorDataReceived(USB_PIPE_t pipe, USB_TRANSFER_STATUS_t status, uint16_t bytesTransferred);
void VendorIsoExtendedBuffer(USB_PIPE_t pipe, USB_TRANSFER_STATUS_t status, uint16_t bytesTransferred);

int main(void)
{
    SYSTEM_Initialize();
    
    RTC_SetPITIsrCallback(RTC_GetUSBState);
    USB_DescriptorPointersSet(&descriptorPointers);
    USB_VendorClassInit(InterfaceEnabledCallback, VendorControlRequest, NULL);
    
    while(1)
    {
        CheckUSBConnection();
    }    
}

void CheckUSBConnection(void) 
{
    /* Handles USB transfers and events,
     * when the AVR is connected to a device (i.e. there is voltage on vbus)
     */
    USBDevice_Handle();
    bool currentVbusState = vbusFlag;

     // Handles USB connects and disconnects
    if (currentVbusState != prevVbusState)
    {
        if(currentVbusState == true)
        {
            status = USB_Start();
        }
        else
        {
            status = USB_Stop();
        }
        prevVbusState = currentVbusState;
    }
    return;
}

// Periodically measuring vbus voltage
void RTC_GetUSBState(void)
{
    static volatile uint8_t PIT_Counter = 0;
    static volatile uint8_t AC_prevState = 0;

    uint8_t AC_currentState = AC0_Read(); // Saves the current state of the AC status register ('1' means above threshold)

    // Makes sure that the AC state has been the same for a certain number of PIT interrupts
    if (PIT_Counter == CONSECUTIVE_EQUAL_PIT_INTERRUPTS)
    {
        if (AC_currentState) // 1 = High, which means that the VBUS is above the threshold and the pull-up on data+ should be enabled.
        {
            vbusFlag = true;
        }
        else
        {
            vbusFlag = false;
        }
        PIT_Counter++; // Increment the counter to not run start/stop multiple times without an actual state change
    }
    // Increments a counter if the AC has been in the same state for a certain number of PIT interrupts
    else if (AC_currentState == AC_prevState)
    {
        if (PIT_Counter <= CONSECUTIVE_EQUAL_PIT_INTERRUPTS + 1)
        {
            PIT_Counter++;
        }
        else
        {
            ; // Stops counting if the AC has been in the same state for a while.
        }
    }
    else
    {
        PIT_Counter = 0; // Resets the PIT counter if a state change has occurred.
    }
    AC_prevState = AC_currentState;
    return;
}

void InterfaceEnabledCallback(USB_SETUP_REQUEST_t *setupRequestPtr)
{
    // 32 interface numbers are supported, so masking out the wIndex
    uint8_t interface = setupRequestPtr->wIndex & 0x1f;
    // 256 alternate interface numers are supported, so masking out the wValue
    uint8_t alternateInterface = setupRequestPtr->wValue & 0xff;

    if (interface == INTERFACE_ENDPOINTS)
    {
        switch (alternateInterface)
        {
            case INTERFACE_ALTERNATE_BULKINT:
            {
                // Start buffer transfer OUT for the bulk and interrupt endpoints, vendor_data_received will call a buffer transfer IN when finished

                USB_PIPE_t interrupt_out_pipe = { .address = INTERFACE0ALTERNATE1_INTERRUPT_EP1_OUT, .direction = USB_EP_DIR_OUT };
                USB_TransferReadStart(interrupt_out_pipe, vendorBuffer, bulkintTransferSize, bulkintUseZLP, VendorDataReceived);

                USB_PIPE_t bulk_out_pipe = { .address = INTERFACE0ALTERNATE1_BULK_EP2_OUT, .direction = USB_EP_DIR_OUT };
                USB_TransferReadStart(bulk_out_pipe, vendorBuffer, bulkintTransferSize, bulkintUseZLP, VendorDataReceived);
                break;
            }
            case INTERFACE_ALTERNATE_ISO1:
            {
                // Start a parallel buffer transfer OUT for Iso endpoint with size <= 512
                USB_PIPE_t iso_out_pipe = { .address = INTERFACE0ALTERNATE2_ISOCHRONOUS_EP3_OUT, .direction = USB_EP_DIR_OUT };
                VendorIsoExtendedBuffer(iso_out_pipe, USB_PIPE_TRANSFER_OK, 0);
                break;
            }
            case INTERFACE_ALTERNATE_ISO1023_OUT:
            {
                // Special OUT transfer for isochronous with 1023 byte endpoint size because OS drivers have difficulties allocating resources for 2x 1023 byte endpoints
                USB_PIPE_t iso_out2_pipe = { .address = INTERFACE0ALTERNATE3_ISOCHRONOUS_EP4_OUT, .direction = USB_EP_DIR_OUT };
                USB_TransferReadStart(iso_out2_pipe, vendorBuffer, VENDOR_ISO_TRANSFER, false, NULL);
                break;
            }
            case INTERFACE_ALTERNATE_ISO1023_IN:
            {
                // Special IN transfer for isochronous with 1023 byte endpoint size because OS drivers have difficulties allocating resources for 2x 1023 byte endpoints
                USB_PIPE_t iso_in2_pipe = { .address = INTERFACE0ALTERNATE4_ISOCHRONOUS_EP4_IN, .direction = USB_EP_DIR_IN };
                USB_TransferWriteStart(iso_in2_pipe, vendorBuffer, VENDOR_ISO_TRANSFER, false, NULL);
                break;
            }
            default:
                break;
        }
    }
}

RETURN_CODE_t VendorControlRequest(USB_SETUP_REQUEST_t *setupRequestPtr)
{
    RETURN_CODE_t status = SUCCESS;

    if (setupRequestPtr->wIndex == REQUEST_VENDOR_TEST_CONTROL) // endpoint control transfer
    {
        // Transfer up to VENDOR_DEFAULT_TRANSFER number of bytes to or from the host,
        // USB_TransferControlDataSet is used for both IN and OUT control data transfers
        if (setupRequestPtr->wLength > VENDOR_DEFAULT_TRANSFER)
        {
            USB_TransferControlDataSet(vendorBuffer, VENDOR_DEFAULT_TRANSFER, NULL);
        }
        else
        {
            USB_TransferControlDataSet(vendorBuffer, setupRequestPtr->wLength, NULL);
        }
    }
    else if (setupRequestPtr->wIndex == REQUEST_VENDOR_CONFIGURE_BULKINT_ZLP) // configure use of zlp
    {
        bulkintUseZLP = setupRequestPtr->wValue & 0x0001;
        
        // Return an empty packet to acknowledge the request
        USB_TransferControlDataSet(NULL, 0, NULL);
    }
    else if (setupRequestPtr->wIndex == REQUEST_VENDOR_CONFIGURE_BULKINT_SIZE) // update bulk transfer size
    {
        bulkintTransferSize = setupRequestPtr->wValue;

        // Read a full endpoint size if we are testing azlp, just to show that the transfer is successful in any case
        if (bulkintUseZLP && (0u != (bulkintTransferSize % INTERFACE0ALTERNATE1_BULK_EP2_IN_SIZE)))
        {
            bulkintTransferSize = ((bulkintTransferSize / INTERFACE0ALTERNATE1_BULK_EP2_IN_SIZE) + 1) * INTERFACE0ALTERNATE1_BULK_EP2_IN_SIZE;
        }
        
        // Return an empty packet to acknowledge the request
        USB_TransferControlDataSet(NULL, 0, NULL);
    }
    else
    {
        status = UNSUPPORTED;
    }

    /* Control requests must return SUCCESS, UNSUPPORTED or an error
     * if returning UNSUPPORTED the core will stall the request
     */
    return status;
}

void VendorDataSent(USB_PIPE_t pipe, USB_TRANSFER_STATUS_t status, uint16_t bytesTransferred)
{
    if (USB_PIPE_TRANSFER_OK == status)
    {
        /* Data sent, expect new data on the IN endpoint, up to VENDOR_DEFAULT_TRANSFER.
         * VendorDataReceived will call a buffer transfer IN when the transfer is finished
         */
        pipe.direction = USB_EP_DIR_OUT;
        USB_TransferReadStart(pipe, vendorBuffer, bulkintTransferSize, bulkintUseZLP, VendorDataReceived);
    }
}

void VendorDataReceived(USB_PIPE_t pipe, USB_TRANSFER_STATUS_t status, uint16_t bytesTransferred)
{
    if (USB_PIPE_TRANSFER_OK == status)
    {
        // Data received, return back the same data on the OUT endpoint
        // vendor_data_sent will call a buffer transfer OUT when the transfer is finished
        pipe.direction = USB_EP_DIR_IN;
        USB_TransferWriteStart(pipe, vendorBuffer, bytesTransferred, bulkintUseZLP, VendorDataSent);
    }
}

void VendorIsoExtendedBuffer(USB_PIPE_t pipe, USB_TRANSFER_STATUS_t status, uint16_t bytesTransferred)
{
    // Start the in and out transfers in parallel, with VENDOR_ISO_TRANSFER size
    // Requires the host to always have the OUT transfer one packet ahead of the IN transfer
    if (USB_PIPE_TRANSFER_OK == status)
    {
        pipe.direction = USB_EP_DIR_IN;
        USB_TransferWriteStart(pipe, vendorBuffer, VENDOR_DEFAULT_TRANSFER, false, VendorIsoExtendedBuffer);

        pipe.direction = USB_EP_DIR_OUT;
        USB_TransferReadStart(pipe, vendorBuffer, VENDOR_DEFAULT_TRANSFER, false, NULL);
    }
}