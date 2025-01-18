;;;//###########################################################################
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

    .def _pBootEntry
    .ref _CRCTestVector
    .ref code_start
    .def _pAppEntry
    .def _pAppSig

***********************************************************************
* Function: codestart section
*
* Description: Branch to code starting point
***********************************************************************

    .sect "app_table"

app_table:
_pAppEntry    .long code_start
_pAppSig      .long _CRCTestVector
    
    .sect "bl_table"

bl_table:
_pBootEntry   .long 0
    

	.end
	
;//===========================================================================
;// End of file.
;//===========================================================================

