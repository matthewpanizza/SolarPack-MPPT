//###########################################################################
//
// bl_check.h - Definitions for the forced update check function.
//
//###########################################################################
// $TI Release: F2806x C/C++ Header Files and Peripheral Examples V151 $
// $Release Date: February  2, 2016 $
// $Copyright: Copyright (C) 2011-2016 Texas Instruments Incorporated -
//             http://www.ti.com/ ALL RIGHTS RESERVED $
//###########################################################################

#ifndef __BL_CHECK_H__
#define __BL_CHECK_H__

//*****************************************************************************
//
// Prototype for the forced update check function.
//
//*****************************************************************************
extern unsigned long CheckForceUpdate(void);
#ifdef ENABLE_UPDATE_CHECK
extern unsigned long CheckGPIOForceUpdate(void);
extern unsigned long g_ulForced;
#endif
#ifdef ENABLE_CRC_CHECK
extern unsigned char CheckCRCTable(volatile CRC_TABLE * pCRCTable);
#endif

#endif // __BL_CHECK_H__


