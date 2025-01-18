//###########################################################################
//
// FILE:   usb_mouse_structs.h
//
// TITLE:  USB mouse device structure definitions.
//
//###########################################################################
// $TI Release: F2806x C/C++ Header Files and Peripheral Examples V151 $
// $Release Date: February  2, 2016 $
// $Copyright: Copyright (C) 2011-2016 Texas Instruments Incorporated -
//             http://www.ti.com/ ALL RIGHTS RESERVED $
//###########################################################################

#ifndef __USB_MOUSE_STRUCTS_H__
#define __USB_MOUSE_STRUCTS_H__

extern unsigned long MouseHandler(void *pvCBData,
                                  unsigned long ulEvent,
                                  unsigned long ulMsgData,
                                  void *pvMsgData);

extern const tUSBDHIDMouseDevice g_sMouseDevice;

#endif // __USB_MOUSE_STRUCTS_H__
