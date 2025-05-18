//----------------------------------------------------------------------------------
//	FILE:			BridgelessPFC-Settings.h
//
//	Description:    This file contains the definitions for this project, and is 
//					linked to both BridgelessPFC-Main.c and BridgelessPFC-DPL-ISR.asm 
//					(where X is the project name).  
//
//	Type: 			Device Independent
//
//----------------------------------------------------------------------------------
//  Copyright Texas Instruments � 2010
//----------------------------------------------------------------------------------
//  Revision History:
//----------------------------------------------------------------------------------
//  Date	  | Description / Status
//----------------------------------------------------------------------------------
// 9 April 2010 - MB
//----------------------------------------------------------------------------------

#ifndef _PROJSETTINGS_H
#define _PROJSETTINGS_H

//**********************************************************************************
//  NOTE: WHEN CHANGING THIS FILE PLEASE REBUILD ALL
//**********************************************************************************

//==================================================================================
// Incremental Build options for System check-out
//==================================================================================
// Open Loop PWMDRV_1ch using PWM 1 on CLA, PWMDRV_CLA_1ch using PWM2 on C28x

//#define INCR_BUILD 1	// Build 1 used for open loop debug of boost stage; LLC always runs under open loop

//#define INCR_BUILD 2  //Build 2 used for boost in closed current & open volt loop. Also used to generate test code for PTI
#define INCR_BUILD 3

#define VoltCurrLoopExecRatio	2
//==================================================================================
// System Settings
//----------------------------------------------------------------------------------
//Add any system specific setting below
#define HistorySize 8
//#define DLOG_SIZE   200

//==================================================================================
// Interrupt Framework options
//==================================================================================

#define EPWMn_DPL_ISR	1	// for EPWM triggered ISR set as 1
#define ADC_DPL_ISR	    0	// for ADC triggered ISR set as 1 

//----------------------------------------------------------------------------------
// If EPWMn_DPL_ISR = 1, then choose which module
//----------------------------------------------------------------------------------
#define EPWM1			1	// EPWM1 provides ISR trigger
#define EPWM2			0 	// EPWM2 provides ISR trigger

#define VPV_FULL_RANGE (512.0) //PV panel voltage that generates 3.3V at ADC input
#define VPV_MIN ((int32)((20.0/VPV_FULL_RANGE)*4096*4096)) //Min PV Panel volt to start MPPT

#define VBUS_FULL_RANGE 	(519.0)//(522.5)//(503.0) //full range of ADC for VBUS
#define VBUS_RATED_VOLTS	(400.0)//(395.0) //395.0V

#define VBUS_OVP_THRSHLD ((int32)((400.0/VBUS_FULL_RANGE)*4096*4096)) //****This parameter not used in this EVM
#define VBUS_TARGET			((int32)((VBUS_RATED_VOLTS/VBUS_FULL_RANGE)*4096*4096)) //395V


#endif //_PROJSETTINGS_H

