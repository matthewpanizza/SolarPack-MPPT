;----------------------------------------------------------------------------------
;	FILE:			Solar_DC_DC-DPL-ISR.asm
;
;	Description:	Solar_DC_DC-DPL-ISR.asm contains the ISR for the system
;					It also contains the initailization routine for all the macros 
;					being used in the system both for CLA and C28x macros
;   
;   Revision/ Version: See Solar_DC_DC-Main.c
;----------------------------------------------------------------------------------

		;Gives peripheral addresses visibility in assembly
	    .cdecls   C,LIST,"PeripheralHeaderIncludes.h"

		;include C header file - sets INCR_BUILD etc.(used in conditional builds)
		.cdecls C,NOLIST, "Solar_DC_DC-Settings.h"

		;Include files for the Power Library Maco's being used by the system
		.include "ADCDRV_1ch.asm"
		.include "PWMDRV_1ch_UpDwnCnt.asm" 
		.include "PWMDRV_LLC_ComplPairDB.asm" 
		.include "CNTL_2P2Z.asm"
		.include "MATH_EMAVG.asm"
			
;=============================================================================
; Digital Power library - Initailization Routine 
;=============================================================================

		; label to DP initialisation function
		.def _DPL_Init	

		; dummy variable for pointer initialisation
ZeroNet	 .usect "ZeroNet_Section",2,1,1	; output terminal 1
VloopCtr .usect "ISRVariables",2,1,1
	

		.text
_DPL_Init:
		ZAPA
		MOVL	XAR0, #ZeroNet
		MOVL	*XAR0, ACC

; Initialize all the DP library macro used here 
;---------------------------------------------------------
		.if(INCR_BUILD = 1)
			ADCDRV_1ch_INIT 1	; IL
			ADCDRV_1ch_INIT 2	; Vb
			ADCDRV_1ch_INIT 3	; Vp
			ADCDRV_1ch_INIT 4	; Ics
			
			MATH_EMAVG_INIT 1
			MATH_EMAVG_INIT 2			
		
			PWMDRV_1ch_UpDwnCnt_INIT 1	; PWM1A	
			PWMDRV_1ch_UpDwnCnt_INIT 2	; PWM2A	
			PWMDRV_1ch_UpDwnCnt_INIT 4	; PWM4A
			
			;PWMDRV_LLC_ComplPairDB_INIT 3;
		.endif
;---------------------------------------------------------------------	
;---------------------------------------------------------
		.if(INCR_BUILD = 2)
			ADCDRV_1ch_INIT 1	; IL
			ADCDRV_1ch_INIT 2	; Vb
			ADCDRV_1ch_INIT 3	; Vp
			ADCDRV_1ch_INIT 4	; Ics

			CNTL_2P2Z_INIT 1
			
			MATH_EMAVG_INIT 1
			MATH_EMAVG_INIT 2			
			
			PWMDRV_1ch_UpDwnCnt_INIT 1	; PWM1A	
			PWMDRV_1ch_UpDwnCnt_INIT 2	; PWM2A	
			PWMDRV_1ch_UpDwnCnt_INIT 4	; PWM4A
			;PWMDRV_LLC_ComplPairDB_INIT 3 ;
		.endif
;---------------------------------------------------------------------		
		.if(INCR_BUILD = 3)
			ADCDRV_1ch_INIT 1	; IL
			ADCDRV_1ch_INIT 2	; Vb
			ADCDRV_1ch_INIT 3	; Vp
			ADCDRV_1ch_INIT 4	; Ics

			CNTL_2P2Z_INIT 2
			
			CNTL_2P2Z_INIT 1
			
			MATH_EMAVG_INIT 1
			MATH_EMAVG_INIT 2			


			PWMDRV_1ch_UpDwnCnt_INIT 1	; PWM1A	
			PWMDRV_1ch_UpDwnCnt_INIT 2	; PWM2A	
			PWMDRV_1ch_UpDwnCnt_INIT 4	; PWM4A
			;PWMDRV_LLC_ComplPairDB_INIT 3 ;
			
		.endif
;---------------------------------------------------------			
		LRETR

;-----------------------------------------------------------------------------------------

; Digital Power library - Interrupt Service Routine

		.sect "ramfuncs"
		; label to DP ISR Run function
		.def	_DPL_ISR

_DPL_ISR:
		; full context save - push any unprotected registers onto stack
		PUSH  	AR1H:AR0H
		PUSH  	XAR2
		PUSH  	XAR3
		PUSH  	XAR4
		PUSH  	XAR5
		PUSH  	XAR6
		PUSH  	XAR7
		PUSH  	XT
		SPM   	0          				; set C28 mode
		CLRC  	AMODE       
		CLRC  	PAGE0,OVM 
;		CLRC	INTM					; clear interrupt mask - comment if ISR non-nestable
;-----------------------------------------------------------------------------------------

; call DP library modules
;---------------------------------------------------------
		.if(INCR_BUILD = 1)
			ADCDRV_1ch 1	; Ipfc
			ADCDRV_1ch 2	; Vpfc
			ADCDRV_1ch 3	; VL_fb
			ADCDRV_1ch 4	; VN_fb
			
			PWMDRV_1ch_UpDwnCnt 1		; PWM1A
			PWMDRV_1ch_UpDwnCnt 2		; PWM2A
			;PWMDRV_LLC_ComplPairDB 3 	;
			
			MATH_EMAVG		1			;Used to average IL
			MATH_EMAVG		2			;Used to average Vb (boost output volt)
			
			PWMDRV_1ch_UpDwnCnt 4		; PWM4A
		.endif
;----------------------------------------------
;---------------------------------------------------------
		.if(INCR_BUILD = 2)
			ADCDRV_1ch 1	; Ipfc
			ADCDRV_1ch 2	; Vpfc
			ADCDRV_1ch 3	; VL_fb
			ADCDRV_1ch 4	; VN_fb

			
			CNTL_2P2Z		1
			
			PWMDRV_1ch_UpDwnCnt 1		; PWM1A
			PWMDRV_1ch_UpDwnCnt 2		; PWM2A
			;PWMDRV_LLC_ComplPairDB 3	;
			
			MATH_EMAVG		1						
		    
			MATH_EMAVG		2
			
			PWMDRV_1ch_UpDwnCnt 4		; PWM4A
		.endif
;----------------------------------------------
		.if(INCR_BUILD = 3)
			ADCDRV_1ch 1	; Ipfc
			ADCDRV_1ch 2	; Vpfc
			ADCDRV_1ch 3	; VL_fb
			ADCDRV_1ch 4	; VN_fb
	
			
			CNTL_2P2Z		1
			
			PWMDRV_1ch_UpDwnCnt 1		; PWM1A
			PWMDRV_1ch_UpDwnCnt 2		; PWM2A
		    ;PWMDRV_LLC_ComplPairDB 3	;
		    
		    
		    MATH_EMAVG		1
			MATH_EMAVG		2
			
			;PWMDRV_1ch_UpDwnCnt 4		; PWM4A
			
;Execute Vloop every VoltCurrLoopExecRatio times, defined in PFC2PhiL-Settings.h file 	
		MOVW	DP,#(VloopCtr)
		INC		@VloopCtr
		CMP		@VloopCtr,#VoltCurrLoopExecRatio
		B		SKIP_VLOOP_CALC,LT
		
		MOV		@VloopCtr,#0
		
		CNTL_2P2Z		2			;Volt loop controller
		
SKIP_VLOOP_CALC:
			
		.endif
;----------------------------------------------------			
		;Subtract IL CS amplifier offset and calculate IL
	        MOVW 	DP, #_AdcResult                     ; load Data Page to read ADC results
        	MOV 	ACC, @_AdcResult.ADCRESULT1<<12		; ACC = IL_raw
        	SUB		ACC, #82<<12		; ACC = IL_raw - offset, IL CS OPAMP offset = 0.026*3.3V = 85.8mV
										; Therefore, 12bit offset value = 0.02*4095 = 82
        	MOVB	XAR2, #0							
			MAXL	ACC, @XAR2			; set min value to zero					
		; Save IL
			.ref	_IL
        	MOVW	DP, #_IL
			MOVL	@_IL, ACC			

			;.endif
		;----------------------------------------------------------
;			.ref 	_Duty4A		
;			MOVW	DP, #_Vrect
;			MOVL	ACC, @_Vrect
;			MOVW	DP, #_Duty4A
;			MOVL    @_Duty4A, ACC	;Write 9 bit value to Duty4A
			
;-----------------------------------------------------------------------------------------
; Interrupt management before exit

	.if(EPWMn_DPL_ISR=1)

		.if(EPWM1)
			MOVW 	DP,#_EPwm1Regs.ETCLR
			MOV 	@_EPwm1Regs.ETCLR,#0x01			; Clear EPWM1 Int flag
		.endif ; EPWM1
	
		.if(EPWM2)
			MOVW 	DP,#_EPwm2Regs.ETCLR
			MOV 	@_EPwm2Regs.ETCLR,#0x01			; Clear EPWM2 Int flag
		.endif ; EPWM2
	
		MOVW 	DP,#_PieCtrlRegs.PIEACK			; Acknowledge PIE interrupt Group 3
		MOV 	@_PieCtrlRegs.PIEACK, #0x4
	.endif ; EPWMn_ISR

	.if(ADC_DPL_ISR=1)
	; Case where ISR is triggered by ADC 
;		MOVW 	DP,#_AdcRegs.ADCINTFLGCLR
;		MOV 	@AdcRegs.ADCINTFLGCLR, #0x01	; Clear ADCINT1 Flag

		MOVW 	DP,#_PieCtrlRegs.PIEACK			; Acknowledge PIE interrupt Group 1
		MOV 	@_PieCtrlRegs.PIEACK, #0x1
	.endif 

;-----------------------------------------------------------------------------------------
; full context restore
;		SETC	INTM					; set INTM to protect context restore
		POP   	XT
		POP   	XAR7
		POP   	XAR6
		POP   	XAR5
		POP   	XAR4
		POP   	XAR3
		POP   	XAR2
		POP   	AR1H:AR0H
		IRET							; return from interrupt
		.end

; end of file

