//#############################################################################
// FILE: usbhostpriv.h
// TITLE: Internal header file for USB host functions
//#############################################################################
// $TI Release: F2806x C/C++ Header Files and Peripheral Examples V151 $
// $Release Date: February  2, 2016 $
// $Copyright: Copyright (C) 2011-2016 Texas Instruments Incorporated -
//             http://www.ti.com/ ALL RIGHTS RESERVED $
//#############################################################################

#ifndef __USBHOSTPRIV_H__
#define __USBHOSTPRIV_H__

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
// The states a hub port can be in during device connection.
//
//*****************************************************************************
typedef enum
{
    //
    // The port has no device connected.
    //
    ePortIdle,

    //
    // The port has a device present and is waiting for the enumeration
    // sequence to begin.
    //
    ePortConnected,

    //
    // A device connection notification has been received and we have initiated
    // a reset to the port.  We are waiting for the reset to complete.
    //
    ePortResetActive,

    //
    // The Port reset has completed but now the hub is waiting the required
    // 10ms before accessing the device.
    //
    ePortResetWait,

    //
    // A device is connected and the port has been reset.  Control has been
    // passed to the main host handling portion of USBLib to enumerate the
    // device.
    //
    ePortActive,

    //
    // A device has completed enumeration.
    //
    ePortEnumerated,

    //
    // A device is attached to the port but enumeration failed.
    //
    ePortError
}
tHubPortState;

//*****************************************************************************
//
// The list of valid event flags in the g_sUSBHCD.ui32EventEnables member
// variable.
//
//*****************************************************************************
#define USBHCD_EVFLAG_SOF       0x00000001
#define USBHCD_EVFLAG_CONNECT   0x00000002
#define USBHCD_EVFLAG_UNKCNCT   0x00000004
#define USBHCD_EVFLAG_DISCNCT   0x00000008
#define USBHCD_EVFLAG_PWRFAULT  0x00000010
#define USBHCD_EVFLAG_PWRDIS    0x00000020
#define USBHCD_EVFLAG_PWREN     0x00000040

//*****************************************************************************
//
//! This is the structure that holds all of the information for devices
//! that are enumerated in the system.   It is passed in to Open function of
//! USB host class drivers so that they can allocate any endpoints and parse
//! out other information that the device class needs to complete enumeration.
//
//*****************************************************************************
struct tUSBHostDevice
{
    //
    //! The current device address for this device.
    //
    uint32_t ui32Address;

    //
    //! The current interface for this device.
    //
    uint32_t ui32Interface;

    //
    //! A flag used to record whether this is a low-speed or a full-speed
    //! device.
    //
    bool bLowSpeed;

    //
    //! A flag indicating whether or not we have read the device's
    //! configuration descriptor yet.
    //
    bool bConfigRead;

    //
    //! The hub number to which this device is attached.
    //
    uint8_t ui8Hub;

    //
    //! The hub port number to which the device is attached.
    //
    uint8_t ui8HubPort;

    //
    //! The device descriptor for this device.
    //
    tDeviceDescriptor sDeviceDescriptor;

    //
    //! A pointer to the configuration descriptor for this device.
    //
    tConfigDescriptor *psConfigDescriptor;

    //
    //! The size of the buffer allocated to psConfigDescriptor.
    //
    uint32_t ui32ConfigDescriptorSize;

    //
    //! Internal flags used by the host controller driver.
    //
    uint32_t ui32Flags;
};

//*****************************************************************************
//
// Functions within the host controller that are called by the hub class driver
//
//*****************************************************************************
extern uint32_t USBHCDHubDeviceConnected(uint32_t ui32Index, uint8_t ui8Hub,
                                         uint8_t ui8Port,
                                         bool bLowSpeed);
extern void USBHCDHubDeviceDisconnected(uint32_t ui32Index,
                                        uint32_t ui32DevIndex);

//*****************************************************************************
//
// Functions in the hub class driver that are called by the host controller.
//
//*****************************************************************************
extern void USBHHubMain(void);
extern void USBHHubInit(void);
extern void USBHHubEnumerationComplete(uint8_t ui8Hub, uint8_t ui8Port);
extern void USBHHubEnumerationError(uint8_t ui8Hub, uint8_t ui8Port);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef __cplusplus
}
#endif

#endif // __USBHOSTPRIV_H__
