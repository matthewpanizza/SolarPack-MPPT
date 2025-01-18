//###########################################################################
//
// FILE:    F2806x_MemCopy.c
//
// TITLE:    Memory Copy Utility
//
// ASSUMPTIONS:
//
//
//
// DESCRIPTION:
//
//          This function will copy the specified memory contents from
//          one location to another.
//
//          Uint16 *SourceAddr        Pointer to the first word to be moved
//                                    SourceAddr < SourceEndAddr
//          Uint16* SourceEndAddr     Pointer to the last word to be moved
//          Uint16* DestAddr          Pointer to the first destination word
//
//          No checks are made for invalid memory locations or that the
//          end address is > then the first start address.
//
//
//###########################################################################
// $TI Release: F2806x C/C++ Header Files and Peripheral Examples V151 $
// $Release Date: February  2, 2016 $
// $Copyright: Copyright (C) 2011-2016 Texas Instruments Incorporated -
//             http://www.ti.com/ ALL RIGHTS RESERVED $
//###########################################################################

#include "DSP28x_Project.h"

#include "boot_loader/bl_config.h"

#pragma CODE_SECTION(MemCopy, "normal")
void MemCopy(Uint16 *SourceAddr, Uint16* SourceEndAddr, Uint16* DestAddr)
{
    while(SourceAddr < SourceEndAddr)
    {
       *DestAddr++ = *SourceAddr++;
    }
    return;
}

#ifdef ENABLE_FAILSAFE
#pragma CODE_SECTION(FailSafeMemCopy, "failsafe")
void FailSafeMemCopy(Uint16 *SourceAddr, Uint16* SourceEndAddr, Uint16* DestAddr)
{
    while(SourceAddr < SourceEndAddr)
    {
       *DestAddr++ = *SourceAddr++;
    }
    return;
}
#endif

//===========================================================================
// End of file.
//===========================================================================


