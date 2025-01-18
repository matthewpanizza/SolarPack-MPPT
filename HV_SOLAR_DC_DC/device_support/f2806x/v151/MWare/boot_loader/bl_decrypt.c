//###########################################################################
//
// bl_decrypt.c - Code for performing an in-place decryption of the firmware
//                image as it is downloaded.
//
//###########################################################################
// $TI Release: F2806x C/C++ Header Files and Peripheral Examples V151 $
// $Release Date: February  2, 2016 $
// $Copyright: Copyright (C) 2011-2016 Texas Instruments Incorporated -
//             http://www.ti.com/ ALL RIGHTS RESERVED $
//###########################################################################

#include "bl_config.h"
#include "boot_loader/bl_decrypt.h"

//*****************************************************************************
//
//! \addtogroup bl_decrypt_api
//! @{
//
//*****************************************************************************
#if defined(ENABLE_DECRYPTION) || defined(DOXYGEN)

//*****************************************************************************
//
//! Performs an in-place decryption of downloaded data.
//!
//! \param pucBuffer is the buffer that holds the data to decrypt.
//! \param ulSize is the size, in bytes, of the buffer that was passed in via
//! the pucBuffer parameter.
//!
//! This function is a stub that could provide in-place decryption of the data
//! that is being downloaded to the device.
//!
//! \return None.
//
//*****************************************************************************
void
DecryptData(unsigned char *pucBuffer, unsigned long ulSize)
{
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
#endif



