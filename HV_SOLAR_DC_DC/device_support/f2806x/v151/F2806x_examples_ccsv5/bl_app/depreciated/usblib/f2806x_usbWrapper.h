//###########################################################################
//
// FILE:   f2806x_usbWrapper.c
//
// TITLE:  Wrapper for interrupt functions and USB support pins.
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
#include "driverlib/usb.h"
#include "usblib/usblib.h"

extern void f2806x_USB0DeviceIntHandler(void);
extern void f2806x_USB0HostIntHandler(void);
extern void f2806x_USB0DualModeIntHandler(void);
extern void f2806x_USBVBusIntHandler(void);
extern void f2806x_USBPFLTIntHandler(void);

extern void USB0DeviceIntHandler(void);
extern void USB0HostIntHandler(void);
extern void USB0DualModeIntHandler(void);
extern void OctaveVBusIntInit(tUSBCallback *Callback);
extern void OctaveExtPowerInit(tUSBCallback *Callback);



