//###########################################################################
//
// FILE:   usbddfu-rt.c
//
// TITLE:  USB Device Firmware Update runtime device class driver.
//
//###########################################################################
// $TI Release: F2806x C/C++ Header Files and Peripheral Examples V151 $
// $Release Date: February  2, 2016 $
// $Copyright: Copyright (C) 2011-2015 Texas Instruments Incorporated -
//             http://www.ti.com/ ALL RIGHTS RESERVED $
//###########################################################################

#include "DSP28x_Project.h"

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/usb.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "usblib/usblib.h"
#include "usblib/usbdfu.h"
#include "usblib/usb-ids.h"
#include "usblib/device/usbdevice.h"
#include "usblib/device/usbddfu-rt.h"
#include "usblib/usblibpriv.h"

//*****************************************************************************
//
//! \addtogroup dfu_device_class_api
//! @{
//
//*****************************************************************************

extern void * pBootEntry;

//*****************************************************************************
//
// DFU Device Descriptor.  This is a dummy structure since runtime DFU must be
// a part of a composite device and cannot be instantiated on its own.
//
//*****************************************************************************
const unsigned char g_pDFUDeviceDescriptor[] =
{
    18,                         // Size of this structure.
    USB_DTYPE_DEVICE,           // Type of this structure.
    USBShort(0x110),            // USB version 1.1 (if we say 2.0, hosts assume
                                // high-speed - see USB 2.0 spec 9.2.6.6)
    USB_CLASS_VEND_SPECIFIC,    // USB Device Class
    0,                          // USB Device Sub-class
    0,                          // USB Device protocol
    64,                         // Maximum packet size for default pipe.
    USBShort(0),                // Vendor ID (VID).
    USBShort(0),                // Product ID (PID).
    USBShort(0),                // Device Release Number BCD.
    0,                          // Manufacturer string identifier.
    0,                          // Product string indentifier.
    0,                          // Product serial number.
    1                           // Number of configurations.
};

//*****************************************************************************
//
// DFU device runtime configuration descriptor.  This is also a dummy structure
// since the primary device class configuration will be used when DFU is added
// to the composite device.
//
//*****************************************************************************
unsigned char g_pDFUConfigDescriptor[] =
{
    //
    // Configuration descriptor header.
    //
    9,                          // Size of the configuration descriptor.
    USB_DTYPE_CONFIGURATION,    // Type of this descriptor.
    USBShort(27),               // The total size of this full structure.
    1,                          // The number of interfaces in this
                                // configuration.
    1,                          // The unique value for this configuration.
    0,                          // The string identifier that describes this
                                // configuration.
    USB_CONF_ATTR_SELF_PWR,     // Bus Powered, Self Powered, remote wake up.
    250,                        // The maximum power in 2mA increments.
};

//*****************************************************************************
//
// The DFU runtime interface descriptor.
//
//*****************************************************************************
unsigned char g_pDFUInterface[] =
{
    //
    // Interface descriptor for runtime DFU operation.
    //
    9,                          // Length of this descriptor.
    USB_DTYPE_INTERFACE,        // This is an interface descriptor.
    0,                          // Interface number .
    0,                          // Alternate setting number.
    0,                          // Number of endpoints (only endpoint 0 used)
    USB_CLASS_APP_SPECIFIC,     // Application specific interface class
    USB_DFU_SUBCLASS,           // Device Firmware Upgrade subclass
    USB_DFU_RUNTIME_PROTOCOL,   // DFU runtime protocol
    0,                          // No string descriptor for this interface.
};

//*****************************************************************************
//
// The DFU functional descriptor.
//
//*****************************************************************************
unsigned char g_pDFUFunctionalDesc[] =
{
    //
    // Device Firmware Upgrade functional descriptor.
    //
    9,                              // Length of this descriptor.
    USB_DFU_FUNC_DESCRIPTOR_TYPE,   // DFU Functional descriptor type
    (DFU_ATTR_CAN_DOWNLOAD |        // DFU attributes.
     DFU_ATTR_CAN_UPLOAD |
     DFU_ATTR_WILL_DETACH |
     DFU_ATTR_MANIFEST_TOLERANT),
    USBShort(0xFFFF),               // Detach timeout (set to maximum).
    USBShort(DFU_TRANSFER_SIZE),    // Transfer size 1KB.
    USBShort(0x0110)                // DFU Version 1.1
};

//*****************************************************************************
//
// The DFU runtime configuration descriptor is defined as two sections.
// These sections are:
//
// 1.  The 9 byte configuration descriptor.
// 2.  The interface descriptor + DFU functional descriptor.
//
//*****************************************************************************
const tConfigSection g_sDFUConfigSection =
{
	sizeof(g_pDFUConfigDescriptor) & 0xFF,
	(sizeof(g_pDFUConfigDescriptor) & 0xFF00) >> 8,
    g_pDFUConfigDescriptor
};

const tConfigSection g_sDFUInterfaceSection =
{
	sizeof(g_pDFUInterface) & 0xFF,
	(sizeof(g_pDFUInterface) & 0xFF00) >> 8,
    g_pDFUInterface
};

const tConfigSection g_sDFUFunctionalDescSection =
{
	sizeof(g_pDFUFunctionalDesc) & 0xFF,
	(sizeof(g_pDFUFunctionalDesc) & 0xFF00) >> 8,
    g_pDFUFunctionalDesc
};

//*****************************************************************************
//
// This array lists all the sections that must be concatenated to make a
// single, complete DFU runtime configuration descriptor.
//
//*****************************************************************************
const tConfigSection *g_psDFUSections[] =
{
    &g_sDFUConfigSection,
    &g_sDFUInterfaceSection,
    &g_sDFUFunctionalDescSection
};

#define NUM_DFU_SECTIONS (sizeof(g_psDFUSections) /                         \
                          sizeof(tConfigSection *))

//*****************************************************************************
//
// The header for the single configuration we support.  This is the root of
// the data structure that defines all the bits and pieces that are pulled
// together to generate the configuration descriptor.
//
//*****************************************************************************
tConfigHeader g_sDFUConfigHeader =
{
    NUM_DFU_SECTIONS,
    g_psDFUSections
};

//*****************************************************************************
//
// Configuration Descriptor.
//
//*****************************************************************************
const tConfigHeader * const g_pDFUConfigDescriptors[] =
{
    &g_sDFUConfigHeader
};

//*****************************************************************************
//
// Forward references for device handler callbacks
//
//*****************************************************************************
static void HandleGetDescriptor(void *pvInstance, tUSBRequest *pUSBRequest);
static void HandleRequest(void *pvInstance, tUSBRequest *pUSBRequest);
static void HandleDevice(void *pvInstance, unsigned long ulRequest,
                         void *pvRequestData);

//*****************************************************************************
//
// The device information structure for the USB DFU devices.
//
//*****************************************************************************
tDeviceInfo g_sDFUDeviceInfo =
{
    //
    // Device event handler callbacks.
    //
    {
        HandleGetDescriptor,    // GetDescriptor
        HandleRequest,          // RequestHandler
        0,                      // InterfaceChange
        0,                      // ConfigChange
        0,                      // DataReceived
        0,                      // DataSentCallback
        0,                      // ResetHandler
        0,                      // SuspendHandler
        0,                      // ResumeHandler
        0,                      // DisconnectHandler
        0,                      // EndpointHandler
        HandleDevice,           // Device handler.
    },
    0,                          // Device descriptor (unused - composite device)
    g_pDFUConfigDescriptors,
    0,                          // Completed during USBDDFUCompositeInit().
    0,                          // Completed during USBDDFUCompositeInit().
    &g_sUSBDefaultFIFOConfig,
    0
};

//*****************************************************************************
//
// Device instance specific handler.  This callback received notifications of
// events related to handling interface, endpoint and string identifiers when
// a device is part of a composite device.  In this case, the only resource we
// need which may be renumbered is the DFU runtime interface.
//
//*****************************************************************************
static void
HandleDevice(void *pvInstance, unsigned long ulRequest, void *pvRequestData)
{
    tDFUInstance *psInst;
    unsigned char *pucData;

    //
    // Get a pointer to our instance data.
    //
    psInst = ((tUSBDDFUDevice *)pvInstance)->psPrivateDFUData;

    //
    // Get a byte pointer to the data.
    //
    pucData = (unsigned char *)pvRequestData;

    //
    // Which request event have we been passed?
    //
    switch(ulRequest)
    {
        //
        // This was an interface change event.
        //
        case USB_EVENT_COMP_IFACE_CHANGE:
        {
            //
            // Save the change to the interface number.
            //
            psInst->ucInterface = pucData[1];
            break;
        }

        //
        // We are not interested in any other event.
        //
        default:
        {
            break;
        }
    }
}

//*****************************************************************************
//
// This function is called by the USB device stack whenever a request for a
// non-standard descriptor is received.
//
// \param pvInstance is the instance data for this request.
// \param pUSBRequest points to the request received.
//
// This call parses the provided request structure and determines which
// descriptor is being requested.  Assuming the descriptor can be found, it is
// scheduled for transmission via endpoint zero.  If the descriptor cannot be
// found, the endpoint is stalled to indicate an error to the host.
//
//*****************************************************************************
static void
HandleGetDescriptor(void *pvInstance, tUSBRequest *pUSBRequest)
{
    unsigned long ulSize;

    ASSERT(pvInstance != 0);

    //
    // Which type of class descriptor are we being asked for?  We only support
    // 1 type - the DFU functional descriptor.
    //
    if(((readShort(&(pUSBRequest->wValue)) >> 8) == USB_DFU_FUNC_DESCRIPTOR_TYPE) &&
       ((readShort(&(pUSBRequest->wValue)) & 0xFF) == 0))
    {
        //
        // If there is more data to send than the host requested then just
        // send the requested amount of data.
        //
        if((unsigned short)g_pDFUFunctionalDesc[0] > readShort(&(pUSBRequest->wLength)))
        {
            ulSize = (unsigned long)readShort(&(pUSBRequest->wLength));
        }
        else
        {
            ulSize = (unsigned long)g_pDFUFunctionalDesc[0];
        }

        //
        // Send the data via endpoint 0.
        //
        USBDCDSendDataEP0(0, g_pDFUFunctionalDesc, ulSize);
    }
    else
    {
        //
        // This was an unknown or invalid request so stall.
        //
        USBDCDStallEP0(0);
    }
}

//*****************************************************************************
//
// This function is called by the USB device stack whenever a non-standard
// request is received.
//
// \param pvInstance is the instance data for this HID device.
// \param pUSBRequest points to the request received.
//
// This call parses the provided request structure.  Assuming the request is
// understood, it is handled and any required response generated.  If the
// request cannot be handled by this device class, endpoint zero is stalled to
// indicate an error to the host.
//
//*****************************************************************************
static void
HandleRequest(void *pvInstance, tUSBRequest *pUSBRequest)
{
    tDFUInstance *psInst;
    const tUSBDDFUDevice *psDevice;

    ASSERT(pvInstance != 0);

    //
    // Which device are we dealing with?
    //
    psDevice = pvInstance;

    //
    // Get a pointer to our instance data.
    //
    psInst = psDevice->psPrivateDFUData;

    //
    // Make sure the request was for this interface.
    //
    if(readShort(&(pUSBRequest->wIndex)) != psInst->ucInterface)
    {
        return;
    }

    //
    // Determine the type of request.
    //
    switch(pUSBRequest->bRequest)
    {
        //
        // We have been asked to detach.  In this case, we call back to the
        // application telling it to tidy up and re-enter the boot loader.  We
        // rely upon it doing this on our behalf since this must be done from a
        // non-interrupt context and this call is most likely in interrupt
        // context.
        //
        case USBD_DFU_REQUEST_DETACH:
        {
            //
            // Tell the application it's time to reenter the boot loader.
            //
            psDevice->pfnCallback(psDevice->pvCBData, USBD_DFU_EVENT_DETACH,
                                  0, (void *)0);
            break;
        }

        //
        // This request was not recognized so stall.
        //
        default:
        {
            USBDCDStallEP0(0);
            break;
        }
    }
}

//*****************************************************************************
//
//! Initializes DFU device operation for a given USB controller.
//!
//! \param ulIndex is the index of the USB controller which is to be
//! initialized for DFU runtime device operation.
//! \param psDevice points to a structure containing parameters customizing
//! the operation of the DFU device.
//!
//! \return Returns NULL on failure or the \e psDevice pointer on success.
//
//*****************************************************************************
void *
USBDDFUCompositeInit(unsigned long ulIndex, const tUSBDDFUDevice *psDevice)
{
    tDFUInstance *psInst;

    //
    // Check parameter validity.
    //
    ASSERT(ulIndex == 0);
    ASSERT(psDevice);
    ASSERT(psDevice->psPrivateDFUData);

    //
    // Initialize the workspace in the passed instance structure.
    //
    psInst = psDevice->psPrivateDFUData;
    psInst->psDevInfo = &g_sDFUDeviceInfo;
    psInst->ulUSBBase = USB0_BASE;
    psInst->bConnected = false;
    psInst->ucInterface = 0;

    //
    // Tag the device information with our device data structure pointer.
    //
    psInst->psDevInfo->pvInstance = (void *)psDevice;

    //
    // Return the pointer to the instance indicating that everything went well.
    //
    return((void *)psDevice);
}

//*****************************************************************************
//
//! Shuts down the DFU device.
//!
//! \param pvInstance is the pointer to the device instance structure as
//! returned by USBDDFUCompositeInit().
//!
//! This function terminates DFU operation for the instance supplied and
//! removes the device from the USB bus.
//!
//! Following this call, the \e pvInstance instance should not me used in any
//! other calls.
//!
//! \return None.
//
//*****************************************************************************
void
USBDDFUCompositeTerm(void *pvInstance)
{
    tDFUInstance *psInst;

    ASSERT(pvInstance);

    //
    // Get a pointer to our instance data.
    //
    psInst = ((tUSBDDFUDevice *)pvInstance)->psPrivateDFUData;

    //
    // Terminate the requested instance.
    //
    USBDCDTerm(0);

    psInst->ulUSBBase = 0;
    psInst->psDevInfo = (tDeviceInfo *)0;
}

//*****************************************************************************
//
//! Removes the current USB device from the bus and transfers control to the
//! DFU boot loader.
//!
//! This function should be called from the application's main loop (i.e. not
//! in interrupt context) following a callback to the USB DFU callback function
//! notifying the application of a DETACH request from the host.  The function
//! will prepare the system to switch to DFU mode and transfer control to the
//! boot loader in preparation for a firmware upgrade from the host.
//!
//! The application must ensure that it has completed all necessary shutdown
//! activities (saved any required data, etc.) before making this call since
//! the function will not return.
//!
//! \return This function does not return.
//
//*****************************************************************************
void
USBDDFUUpdateBegin(void)
{
    //
    // Terminate the USB device and take us off the bus.
    //
    USBDCDTerm(0);

    //
    // Disable all interrupts.
    //
    IntMasterDisable();

    //
    // We must make sure we turn off SysTick and its interrupt
    // before entering the boot loader!
    //
    SysTickIntDisable();
    SysTickDisable();

    //
    // Disable all processor interrupts.  Instead of disabling them
    // one at a time, a direct write to PIE is done to disable all
    // peripheral interrupts.
    //
    IER = 0;

    //
    // Reset the USB peripheral
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_USB0);
    SysCtlPeripheralReset(SYSCTL_PERIPH_USB0);
    SysCtlPeripheralDisable(SYSCTL_PERIPH_USB0);

    //
    // Wait for about a second.
    //
    SysCtlDelay(SysCtlClockGet() / 20);

    //
    // Re-enable interrupts at the NVIC level.
    //
    IntMasterEnable();

    //
    // Return control to the boot loader.  This is a call to the SVC
    // handler in the boot loader.
    //
    ((void (*)(void))pBootEntry)();

    //
    // Should never get here, but just in case.
    //
    while(1)
    {
    }
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
