;//###########################################################################
;//
;// FILE:  bl_entrytrable.asm
;//
;// TITLE: Symbol tables for entry points
;//
;// 
;//###########################################################################
;// $TI Release: F2806x C/C++ Header Files and Peripheral Examples V151 $ 
;// $Release Date: February  2, 2016 $ 
;// $Copyright: Copyright (C) 2011-2016 Texas Instruments Incorporated -
;//             http://www.ti.com/ ALL RIGHTS RESERVED $
;//###########################################################################

***********************************************************************

    .ref _AppUpdaterUSB
    .def _pAppEntry
    .def _pAppSig

***********************************************************************
* Function: codestart section
*
* Description: Branch to code starting point
***********************************************************************

    .sect "app_table"

app_table:
_pAppEntry    .long 0
_pAppSig      .long 0
    
    .sect "bl_table"

bl_table:
_pBootEntry   .long _AppUpdaterUSB
    

    .end
    
;//===========================================================================
;// End of file.
;//===========================================================================


