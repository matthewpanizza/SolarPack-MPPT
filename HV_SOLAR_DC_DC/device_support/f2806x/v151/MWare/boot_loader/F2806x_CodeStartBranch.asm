;;;//###########################################################################
;//
;// FILE:  F2806x_CodeStartBranch.asm    
;//
;// TITLE: Branch for redirecting code execution after boot. 
;//
;// For these examples, code_start is the first code that is executed after
;// exiting the boot ROM code. 
;//
;// The codestart section in the linker cmd file is used to physically place
;// this code at the correct memory location.  This section should be placed 
;// at the location the BOOT ROM will re-direct the code to.  For example, 
;// for boot to FLASH this code will be located at 0x3f7ff6. 
;//
;// In addition, the example F2806x projects are setup such that the codegen
;// entry point is also set to the code_start label.  This is done by linker 
;// option -e in the project build options.  When the debugger loads the code,
;// it will automatically set the PC to the "entry point" address indicated by
;// the -e linker option.  In this case the debugger is simply assigning the PC, 
;// it is not the same as a full reset of the device. 
;// 
;// The compiler may warn that the entry point for the project is other then
;//  _c_init00.  _c_init00 is the C environment setup and is run before 
;// main() is entered. The code_start code will re-direct the execution 
;// to _c_init00 and thus there is no worry and this warning can be ignored. 
;// 
;//###########################################################################
;// $TI Release: F2806x C/C++ Header Files and Peripheral Examples V151 $ 
;// $Release Date: February  2, 2016 $ 
;// $Copyright: Copyright (C) 2011-2016 Texas Instruments Incorporated -
;//             http://www.ti.com/ ALL RIGHTS RESERVED $
;//###########################################################################
    .cdecls "boot_loader/bl_config.h"
***********************************************************************

WD_DISABLE    .set    1        ;set to 1 to disable WD, else set to 0

    .ref _c_int00
    .global _code_start_n
    .global normal

 .if $defined("ENABLE_FAILSAFE")    
    .ref _c_int00f
    .global _code_start_f
    .global failsafe
 .endif

***********************************************************************
* Function: codestart section
*
* Description: Branch to code starting point
***********************************************************************

    .sect "codestart"
        LB _code_start_n
    
    .sect "codestart_n"

_code_start_n:
    .if WD_DISABLE == 1
        LB wd_disable       ;Branch to watchdog disable code
    .else
        LB _c_int00         ;Branch to start of boot.asm in RTS library
    .endif

 .if $defined("ENABLE_FAILSAFE")  
    
    .sect "codestart_f"

_code_start_f:
    .if WD_DISABLE == 1
        LB wd_disable_f       ;Branch to watchdog disable code
    .else
        LB _c_int00f         ;Branch to start of boot.asm in RTS library
    .endif
 .endif

;end codestart section

***********************************************************************
* Function: wd_disable
*
* Description: Disables the watchdog timer
***********************************************************************
    .if WD_DISABLE == 1

    .sect "normal"
wd_disable:
    SETC OBJMODE        ;Set OBJMODE for 28x object code
    EALLOW              ;Enable EALLOW protected register access
    MOVZ DP, #7029h>>6  ;Set data page for WDCR register
    MOV @7029h, #0068h  ;Set WDDIS bit in WDCR to disable WD
    EDIS                ;Disable EALLOW protected register access
    LB _c_int00         ;Branch to start of boot.asm in RTS library


 .if $defined("ENABLE_FAILSAFE")      
    .sect "failsafe"
wd_disable_f:
    SETC OBJMODE        ;Set OBJMODE for 28x object code
    EALLOW              ;Enable EALLOW protected register access
    MOVZ DP, #7029h>>6  ;Set data page for WDCR register
    MOV @7029h, #0068h  ;Set WDDIS bit in WDCR to disable WD
    EDIS                ;Disable EALLOW protected register access
    LB _c_int00f         ;Branch to start of boot.asm in RTS library
 .endif   
 .endif
    


;end wd_disable

    .end
    
;//===========================================================================
;// End of file.
;//===========================================================================


