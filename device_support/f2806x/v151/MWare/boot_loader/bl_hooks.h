//###########################################################################
//
// bl_hooks.h - Definitions for the application-specific hook function.
//
//###########################################################################
// $TI Release: F2806x C/C++ Header Files and Peripheral Examples V151 $
// $Release Date: February  2, 2016 $
// $Copyright: Copyright (C) 2011-2016 Texas Instruments Incorporated -
//             http://www.ti.com/ ALL RIGHTS RESERVED $
//###########################################################################

#ifndef __BL_HOOKS_H__
#define __BL_HOOKS_H__

//*****************************************************************************
//
// Prototypes for any application-specific hook functions that are defined in
// bl_config.h.  
//
//*****************************************************************************
#ifdef BL_HW_INIT_FN_HOOK
extern void BL_HW_INIT_FN_HOOK(void);
#endif
#ifdef BL_INIT_FN_HOOK
extern void BL_INIT_FN_HOOK(void);
#endif
#ifdef BL_REINIT_FN_HOOK
extern void BL_REINIT_FN_HOOK(void);
#endif
#ifdef BL_START_FN_HOOK
extern void BL_START_FN_HOOK(void);
#endif
#ifdef BL_PROGRESS_FN_HOOK
extern void BL_PROGRESS_FN_HOOK(unsigned long ulCompleted,
                                unsigned long ulTotal);
#endif
#ifdef BL_END_FN_HOOK
extern void BL_END_FN_HOOK(void);
#endif
#ifdef BL_DECRYPT_FN_HOOK
extern void BL_DECRYPT_FN_HOOK(unsigned char *pucBuffer, unsigned long ulSize);
#endif
#ifdef BL_CHECK_UPDATE_FN_HOOK
extern unsigned long BL_CHECK_UPDATE_FN_HOOK(void);
#endif

//*****************************************************************************
//
// If ENABLE_DECRYPTION is defined but we don't have a hook function set for
// decryption, default to the previous behavior which calls the stub function
// DecryptData.
//
//*****************************************************************************
#if (defined ENABLE_DECRYPTION) && !(defined BL_DECRYPT_FN_HOOK)
#define BL_DECRYPT_FN_HOOK      DecryptData
#endif

#endif // __BL_HOOKS_H__


