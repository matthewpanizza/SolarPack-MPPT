//###########################################################################
//
// FILE:   usb_keyb_structs.h
//
// TITLE:  USB keyboard device structure definitions.
//
//###########################################################################
// $TI Release: F2806x C/C++ Header Files and Peripheral Examples V151 $
// $Release Date: February  2, 2016 $
// $Copyright: Copyright (C) 2011-2016 Texas Instruments Incorporated -
//             http://www.ti.com/ ALL RIGHTS RESERVED $
//###########################################################################

#ifndef __USB_KEYB_STRUCTS_H__
#define __USB_KEYB_STRUCTS_H__

extern unsigned long KeyboardHandler(void *pvCBData,
                                     unsigned long ulEvent,
                                     unsigned long ulMsgData,
                                     void *pvMsgData);

extern tUSBDHIDKeyboardDevice g_sKeyboardDevice;

#endif // __USB_KEYB_STRUCTS_H__
