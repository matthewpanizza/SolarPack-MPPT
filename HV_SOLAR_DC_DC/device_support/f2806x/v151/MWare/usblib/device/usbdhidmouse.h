//#############################################################################
// FILE: usbdhidmouse.h
// TITLE: Public header file for the USB HID Mouse device class
//#############################################################################
// $TI Release: F2806x C/C++ Header Files and Peripheral Examples V151 $
// $Release Date: February  2, 2016 $
// $Copyright: Copyright (C) 2011-2016 Texas Instruments Incorporated -
//             http://www.ti.com/ ALL RIGHTS RESERVED $
//#############################################################################

#ifndef __USBDHIDMOUSE_H__
#define __USBDHIDMOUSE_H__

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef __cplusplus
extern "C"
{
#endif

//*****************************************************************************
//
//! \addtogroup hid_mouse_device_class_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// PRIVATE
//
// The first few sections of this header are private defines that are used by
// the USB HID mouse code and are here only to help with the application
// allocating the correct amount of memory for the HID mouse device code.
//
//*****************************************************************************

//*****************************************************************************
//
// PRIVATE
//
// The size of the mouse input report sent to the host.
//
//*****************************************************************************
#define MOUSE_REPORT_SIZE       3

//*****************************************************************************
//
// PRIVATE
//
// This enumeration holds the various states that the mouse can be in during
// normal operation.
//
//*****************************************************************************
typedef enum
{
    //
    // Unconfigured.
    //
    eHIDMouseStateUnconfigured,

    //
    // No keys to send and not waiting on data.
    //
    eHIDMouseStateIdle,

    //
    // Waiting on report data from the host.
    //
    eHIDMouseStateWaitData,

    //
    // Waiting on data to be sent out.
    //
    eHIDMouseStateSend
}
tMouseState;

//*****************************************************************************
//
// PRIVATE
//
// This structure provides the private instance data structure for the USB
// HID Mouse device.  This structure forms the RAM workspace used by each
// instance of the mouse.
//
//*****************************************************************************
typedef struct
{
    //
    // The USB configuration number set by the host or 0 of the device is
    // currently unconfigured.
    //
    uint8_t ui8USBConfigured;

    //
    // The protocol requested by the host, USB_HID_PROTOCOL_BOOT or
    // USB_HID_PROTOCOL_REPORT.
    //
    uint8_t ui8Protocol;

    //
    // A buffer used to hold the last input report sent to the host.
    //
    uint8_t pui8Report[MOUSE_REPORT_SIZE];

    //
    // The current state of the mouse interrupt IN endpoint.
    //
    volatile tMouseState iMouseState;

    //
    // The idle timeout control structure for our input report.  This is
    // required by the lower level HID driver.
    //
    tHIDReportIdle sReportIdle;

    //
    // This is needed for the lower level HID driver.
    //
    tUSBDHIDDevice sHIDDevice;
}
tHIDMouseInstance;

//*****************************************************************************
//
//! This structure is used by the application to define operating parameters
//! for the HID mouse device.
//
//*****************************************************************************
typedef struct
{
    //
    //! The vendor ID that this device is to present in the device descriptor.
    //
    const uint16_t ui16VID;

    //
    //! The product ID that this device is to present in the device descriptor.
    //
    const uint16_t ui16PID;

    //
    //! The maximum power consumption of the device, expressed in milliamps.
    //
    const uint16_t ui16MaxPowermA;

    //
    //! Indicates whether the device is self- or bus-powered and whether or not
    //! it supports remote wakeup.  Valid values are USB_CONF_ATTR_SELF_PWR or
    //! USB_CONF_ATTR_BUS_PWR, optionally ORed with USB_CONF_ATTR_RWAKE.
    //
    const uint8_t ui8PwrAttributes;

    //
    //! A pointer to the callback function which will be called to notify
    //! the application of events relating to the operation of the mouse.
    //
    const tUSBCallback pfnCallback;

    //
    //! A client-supplied pointer which will be sent as the first
    //! parameter in all calls made to the mouse callback, pfnCallback.
    //
    void *pvCBData;

    //
    //! A pointer to the string descriptor array for this device.  This array
    //! must contain the following string descriptor pointers in this order.
    //! Language descriptor, Manufacturer name string (language 1), Product
    //! name string (language 1), Serial number string (language 1),HID
    //! Interface description string (language 1), Configuration description
    //! string (language 1).
    //!
    //! If supporting more than 1 language, the descriptor block (except for
    //! string descriptor 0) must be repeated for each language defined in the
    //! language descriptor.
    //
    const uint8_t * const *ppui8StringDescriptors;

    //
    //! The number of descriptors provided in the ppStringDescriptors
    //! array.  This must be (1 + (5 * (num languages))).
    //
    const uint32_t ui32NumStringDescriptors;

    //
    //! The private instance data for this device.  This memory must
    //! remain accessible for as long as the mouse device is in use and must
    //! not be modified by any code outside the HID mouse driver.
    //
    tHIDMouseInstance * sPrivateData;
}
tUSBDHIDMouseDevice;

//*****************************************************************************
//
//! This return code from USBDHIDMouseStateChange indicates success.
//
//*****************************************************************************
#define MOUSE_SUCCESS           0

//*****************************************************************************
//
//! This return code from USBDHIDMouseStateChange indicates that an error was
//! reported while attempting to send a report to the host.  A client should
//! assume that the host has disconnected if this return code is seen.
//
//*****************************************************************************
#define MOUSE_ERR_TX_ERROR      2

//*****************************************************************************
//
//! USBDHIDMouseStateChange returns this value if it is called before the
//! USB host has connected and configured the device.  All mouse state
//! information passed on the call will have been ignored.
//
//*****************************************************************************
#define MOUSE_ERR_NOT_CONFIGURED \
                                4

//*****************************************************************************
//
//! Setting this bit in the ui8Buttons parameter to USBDHIDMouseStateChange
//! indicates to the USB host that button 1 on the mouse is pressed.
//
//*****************************************************************************
#define MOUSE_REPORT_BUTTON_1   0x01

//*****************************************************************************
//
//! Setting this bit in the ui8Buttons parameter to USBDHIDMouseStateChange
//! indicates to the USB host that button 2 on the mouse is pressed.
//
//*****************************************************************************
#define MOUSE_REPORT_BUTTON_2   0x02

//*****************************************************************************
//
//! Setting this bit in the ui8Buttons parameter to USBDHIDMouseStateChange
//! indicates to the USB host that button 3 on the mouse is pressed.
//
//*****************************************************************************
#define MOUSE_REPORT_BUTTON_3   0x04

//*****************************************************************************
//
// API Function Prototypes
//
//*****************************************************************************
extern void *USBDHIDMouseInit(uint32_t ui32Index,
                              tUSBDHIDMouseDevice *psMouseDevice);
extern void *USBDHIDMouseCompositeInit(uint32_t ui32Index,
                                     tUSBDHIDMouseDevice *psMouseDevice,
                                     tCompositeEntry *psCompEntry);
extern void USBDHIDMouseTerm(void *pvMouseDevice);
extern void *USBDHIDMouseSetCBData(void *pvMouseDevice, void *pvCBData);
extern uint32_t USBDHIDMouseStateChange(void *pvMouseDevice, int8_t i8DeltaX,
                                        int8_t i8DeltaY, uint8_t ui8Buttons);
extern void USBDHIDMousePowerStatusSet(void *pvMouseDevice,
                                       uint8_t ui8Power);
extern bool USBDHIDMouseRemoteWakeupRequest(void *pvMouseDevice);

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // __USBDHIDMOUSE_H__
