//###########################################################################
//
// bl_check.c - Code to check for a forced update.
//
//###########################################################################
// $TI Release: F2806x C/C++ Header Files and Peripheral Examples V151 $
// $Release Date: February  2, 2016 $
// $Copyright: Copyright (C) 2011-2016 Texas Instruments Incorporated -
//             http://www.ti.com/ ALL RIGHTS RESERVED $
//###########################################################################

#include "DSP28x_Project.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "crc.h"
#include "crc_tbl.h"
#include "bl_config.h"
#include "bl_sym.h"
#include "boot_loader/bl_check.h"
#include "boot_loader/bl_hooks.h"

//*****************************************************************************
//
//! \addtogroup bl_check_api
//! @{
//
//*****************************************************************************

//*****************************************************************************
//
// This global is used to remember if a forced update occurred.
//
//*****************************************************************************
#ifdef ENABLE_UPDATE_CHECK
unsigned long g_ulForced;
#endif

//*****************************************************************************
//
//! Checks a GPIO for a forced update.
//!
//! This function checks the state of a GPIO to determine if a update is being
//! requested.
//!
//! \return Returns a non-zero value if an update is being requested and zero
//! otherwise.
//
//*****************************************************************************
#ifdef ENABLE_UPDATE_CHECK
#pragma CODE_SECTION(CheckGPIOForceUpdate, "normal")
unsigned long
CheckGPIOForceUpdate(void)
{
    EALLOW;
    
    //
    // Wait a while before accessing the peripheral.
    //
    DELAY_US(3);

    //
    // Ensure the selected pin is an input
    //
#if FORCED_UPDATE_PIN > 31
    GpioCtrlRegs.GPBDIR.all &= ~(1 << (FORCED_UPDATE_PIN - 32));
#else
    GpioCtrlRegs.GPADIR.all &= ~(1 << FORCED_UPDATE_PIN);
#endif    

    
#ifdef FORCED_UPDATE_WPU
    //
    // Enable the weak pull up.
    //
#if FORCED_UPDATE_PIN > 31
    GpioCtrlRegs.GPBPUD.all &= ~(1 << (FORCED_UPDATE_PIN - 32));
#else
    GpioCtrlRegs.GPAPUD.all &= ~(1 << FORCED_UPDATE_PIN);
#endif       

#endif

    //
    // Wait a while before reading the pin.
    //
    DELAY_US(10000);

    //
    // Check the pin to see if an update is being requested.
    //
#if FORCED_UPDATE_PIN > 31
    if(((GpioDataRegs.GPBDAT.all >> (FORCED_UPDATE_PIN - 32)) & 0x01) == FORCED_UPDATE_POLARITY)
#else
    if(((GpioDataRegs.GPBDAT.all >> FORCED_UPDATE_PIN) & 0x01) == FORCED_UPDATE_POLARITY)
#endif       
    {
        //
        // Remember that this was a forced update.
        //
        g_ulForced = 1;

        return(1);
    }

    //
    // No update is being requested so return 0.
    //
    return(0);
}
#endif



//*****************************************************************************
//
//! Checks a Linker Generated CRC table against memory
//!
//! \param pCRCTable CRC Table to be checked
//!
//! This function iterates through a CRC table performing the checks specified 
//! against the memory map.
//!
//! \return Returns 1 on failure, 0 on success
//
//*****************************************************************************
#ifdef ENABLE_CRC_CHECK
#pragma CODE_SECTION(CheckCRCTable, "normal")
unsigned char CheckCRCTable(volatile CRC_TABLE * pCRCTable)
{
    volatile unsigned long ulIdx, ulTemp;
    volatile CRC_RECORD * pCRCRecord;
    
    //Quick Sanity Check
    if(((unsigned long)pCRCTable == 0xFFFFFFFF) || (pCRCTable == 0))
        return 1;    
    
//Initialize CRC tables if doing SW CRC    
#ifndef ENABLE_VCU  
  #ifdef GEN_CRC_TABLES  
    #ifdef CRC8
        genCRC8Table();            
    #endif
    #ifdef CRC16P1
        genCRC16P1Table();
    #endif
    #ifdef CRC16P2
        genCRC16P2Table();
    #endif
    #ifdef CRC32
        genCRC32Table();
    #endif    
  #endif
#endif

    for(ulIdx = 0; ulIdx < pCRCTable->num_recs; ulIdx++)
    {
        //Loop through and check each record    
        pCRCRecord = (CRC_RECORD *)&(pCRCTable->recs[ulIdx]);
        
        switch(pCRCRecord->crc_alg_ID)
        {
#ifdef CRC8
            case CRC8_PRIME:
            {
                ulTemp = getCRC8(0, (uint16 *)pCRCRecord->addr, EVEN, 2*pCRCRecord->size);
                if(ulTemp != pCRCRecord->crc_value)
                    return 1;
                break;                
            }            
#endif
#ifdef CRC16P1
            case CRC16_802_15_4:
            {
                ulTemp = getCRC16P1(0, (uint16 *)pCRCRecord->addr, EVEN, 2*pCRCRecord->size);
                if(ulTemp != pCRCRecord->crc_value)
                    return 1;
                break;        
            }    
#endif
#ifdef CRC16P2
            case CRC16_ALT:
            {
                ulTemp = getCRC16P2(0, (uint16 *)pCRCRecord->addr, EVEN, 2*pCRCRecord->size);
                if(ulTemp != pCRCRecord->crc_value)
                    return 1;
                break;        
            }    
#endif
#ifdef CRC32
            case CRC32_PRIME:
            {
                ulTemp = (unsigned long) getCRC32(0, (uint16 *)pCRCRecord->addr, EVEN, 2 * pCRCRecord->size);
                if(ulTemp != pCRCRecord->crc_value)
                    return 1;
                break;        
            }    
#endif        
            default:
            {
                return 1;    
            }        
        }
    }
    //All CRC passed
    return 0;
}
#endif

//*****************************************************************************
//
//! Checks if an update is needed or is being requested.
//!
//! This function detects if an update is being requested or if there is no
//! valid code presently located on the microcontroller.  This is used to tell
//! whether or not to enter update mode.
//!
//! \return Returns a non-zero value if an update is needed or is being
//! requested and zero otherwise.
//
//*****************************************************************************
#pragma CODE_SECTION(CheckForceUpdate, "normal")
unsigned long
CheckForceUpdate(void)
{
#ifdef BL_CHECK_UPDATE_FN_HOOK
    //
    // If the update check function is hooked, call the application to determine
    // how to proceed.
    //
    return(BL_CHECK_UPDATE_FN_HOOK());
#endif

#ifdef ENABLE_UPDATE_CHECK
    g_ulForced = 0;
#endif

    //
    // See if the first location is 0xfffffffff or something that does not
    // look like a stack pointer, or if the second location is 0xffffffff or
    // something that does not look like a reset vector.
    //
    if(((unsigned long) pAppEntry == 0xFFFFFFFF) || ((unsigned long)pAppEntry == 0))
    {
        return(1);
    }

#ifdef ENABLE_UPDATE_CHECK
    //
    // If simple GPIO checking is configured, determine whether or not to force
    // an update.
    //
    return(CheckGPIOForceUpdate());
#else
    //
    // GPIO checking is not required so, if we get here, a valid image exists
    // and no update is needed.
    //
    return(0);
#endif
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************


