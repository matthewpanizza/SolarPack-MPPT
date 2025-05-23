//###########################################################################
// Description:
//
// Compare observed output against expected output vector and print a 
// PASS/FAIL report to the console
//
//###########################################################################
// $TI Release: F2806x C/C++ Header Files and Peripheral Examples V151 $
// $Release Date: February  2, 2016 $
// $Copyright: Copyright (C) 2011-2016 Texas Instruments Incorporated -
//             http://www.ti.com/ ALL RIGHTS RESERVED $
//###########################################################################

#include <stdio.h>
#include <math.h>
#include "DSP28x_Project.h"
#include "F2806x_Cla_defines.h"  
// Include the test header file whose name is based on the test name
// which is defined by the macro TEST on the command line
#include XSTRINGIZE(XCONCAT(TEST_NAME,_shared.h))

void test_report(void)
{
	Uint16 fail_flag = 0x0000;
	Uint16 i;
	float vector1_gold[] = {-11.3, 1.0, 2.5, 6.2, 10.8};
	float vector2_gold[] = {9.9, 8.8, 4.4, 3.3, 2.2, 1.0, 0.0,-8.8,-9.9,-10.1};
    int32 vector3_gold[] = {9,8,4,3,2,1,0,-8,-9};  	
	
    for(i=0; i < LENGTH1; i++)
    {
        if(vector1_sorted[i] != vector1_gold[i]) fail_flag = fail_flag | M_INT1;
    }    
    for(i=0; i < LENGTH2; i++)
    {
        if(vector2_sorted[i] != vector2_gold[i]) fail_flag = fail_flag | M_INT2;
    }
    // 
    // Map the data memory back to the CPU so we can check the
    // results of task 3
    //    
    EALLOW;
    Cla1Regs.MMEMCFG.bit.RAM0E	= 0;
    Cla1Regs.MMEMCFG.bit.RAM1E	= 0;
    EDIS;
   __asm("   NOP");
   __asm("   NOP");
   __asm("   NOP");
   __asm("   NOP");       
       
    for(i=0; i < LENGTH3; i++)
    {
        if(vector3[i] != vector3_gold[i]) fail_flag = fail_flag | M_INT3;
    }      
    if(fail_flag == 0) puts("PASS\n");
    else
       printf("FAIL  fail_flag = 0x%d\n",fail_flag);
        
}
