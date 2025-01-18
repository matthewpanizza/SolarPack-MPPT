//###########################################################################
//
// bl_sym.h - Miscellaneous externs and symbol definitions needed by the 
//            bootloader
//
//###########################################################################
// $TI Release: F2806x C/C++ Header Files and Peripheral Examples V151 $
// $Release Date: February  2, 2016 $
// $Copyright: Copyright (C) 2011-2016 Texas Instruments Incorporated -
//             http://www.ti.com/ ALL RIGHTS RESERVED $
//###########################################################################

#ifndef __BL_APP_H__
#define __BL_APP_H__



//*****************************************************************************
//
// Basic functions for erasing and programming internal flash.
//
//*****************************************************************************

extern void * pAppSig;
extern void * pAppEntry;

extern Uint16 BLTableStart;
extern Uint16 BLTableEnd;
extern Uint16 BLTableSize;
extern Uint16 InitLoadStart;
extern Uint16 InitLoadEnd;
extern Uint16 InitLoadSize;
extern void * code_start_n;
extern Uint16 CodeStart_n_Start;
extern Uint16 CodeStart_n_End;

#ifdef ENABLE_FAILSAFE
extern Uint16 FailsafeLoadStart;
extern Uint16 FailsafeLoadEnd;
extern Uint16 FailsafeLoadSize;
extern Uint16 FailsafeRunStart;
extern void * code_start_f;
extern Uint16 CodeStart_f_Start;
extern Uint16 CodeStart_f_End;
extern void FailSafeMemCopy(Uint16 *SourceAddr, Uint16* SourceEndAddr, Uint16* DestAddr);
#endif


#endif // __BL_APP_H__


