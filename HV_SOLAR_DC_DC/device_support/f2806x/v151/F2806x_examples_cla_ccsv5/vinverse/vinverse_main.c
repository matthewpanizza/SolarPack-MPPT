//###########################################################################
// Description:
//! \addtogroup f2806x_example_cla_list
//! <h1> Vector Inverse</h1>
//!
//! Task 1 calculates the element-wise inverse of a vector while
//! Task 2 calculates the element-wise inverse of a vector and 
//! saves the result in the same vector
//!
//! \b Watch \b Variables \n
//!    - vector1 - Input vector to task 1
//!    - vector1_inverse - Inverse of input vector1
//!    - vector2 - Input/Output vector for task 2
//
//###########################################################################
// $TI Release: F2806x C/C++ Header Files and Peripheral Examples V151 $
// $Release Date: February  2, 2016 $
// $Copyright: Copyright (C) 2011-2016 Texas Instruments Incorporated -
//             http://www.ti.com/ ALL RIGHTS RESERVED $
//###########################################################################

#include "DSP28x_Project.h"     // DSP28x Headerfile
#include "F2806x_Cla_defines.h"  
// Include the test header file whose name is based on the test name
// which is defined by the macro TEST on the command line
#include XSTRINGIZE(XCONCAT(TEST_NAME,_shared.h))

#define CLA_PROG_ENABLE      0x0001
#define CLARAM0_ENABLE       0x0010
#define CLARAM1_ENABLE       0x0020
#define CLARAM2_ENABLE       0x0040
#define CLA_RAM0CPUE         0x0100
#define CLA_RAM1CPUE         0x0200
#define CLA_RAM2CPUE         0x0400

#define HARDWARE 1


// Constants
#define OSCCLK	     (10000000UL) 	         // < Internal OSC frequency
#define SYSCLK	     (80000000UL) 			 // < System Clock frequency
#define PER_CLK_DIV  (4)					 // < Low speed peripheral clock dividers
#define PER_CLK	     (SYSCLK/PER_CLK_DIV)	 // < Low speed peripheral clock
#define DIV_SEL	     (2)		             // < Clock divider selection
#define CLK_DIV	     ((SYSCLK * DIV_SEL)/OSCCLK) // < Clock divider


//CLA ISRs
__interrupt void cla1_task1_isr( void );
__interrupt void cla1_task2_isr( void );
__interrupt void cla1_task3_isr( void );
__interrupt void cla1_task4_isr( void );
__interrupt void cla1_task5_isr( void );
__interrupt void cla1_task6_isr( void );
__interrupt void cla1_task7_isr( void );
__interrupt void cla1_task8_isr( void );

//
// These are defined by the linker file and used to copy
// the CLA code from its load address to its run address
// in CLA program memory
//
extern Uint16 Cla1funcsLoadStart;
extern Uint16 Cla1funcsLoadEnd;
extern Uint16 Cla1funcsLoadSize;
extern Uint16 Cla1funcsRunStart;
extern Uint16 Cla1Prog_Start;
//
// These are defined by the linker file and used to copy
// the CLA math tables from its load address to its run address
// into one of the CLA data RAMs
//
extern Uint16 Cla1mathTablesLoadStart;
extern Uint16 Cla1mathTablesLoadEnd;
extern Uint16 Cla1mathTablesLoadSize;
extern Uint16 Cla1mathTablesRunStart;

//  Main Function
void main(void)
{
   //  Step 1: Setup the system clock
   /*  Disable the watchdog timer, initialize the system clock,
    *  PLL and configure the peripheral clock. 
    */
   InitSysCtrl();
	
   //  Step 2: Initialize PIE control
   /*  Intialize PIE control, disable all interrupts and
    * then copy over the PIE Vector table from BootROM to RAM
    */
   DINT;
   InitPieCtrl();
   IER = 0x00000000;
   IFR = 0x00000000;
   InitPieVectTable();
	
   /*  Assign user defined ISR to the PIE vector table */
	EALLOW;
   PieVectTable.CLA1_INT1  = &cla1_task1_isr;
   PieVectTable.CLA1_INT2  = &cla1_task2_isr;
   PieVectTable.CLA1_INT3  = &cla1_task3_isr;
   PieVectTable.CLA1_INT4  = &cla1_task4_isr;
   PieVectTable.CLA1_INT5  = &cla1_task5_isr;
   PieVectTable.CLA1_INT6  = &cla1_task6_isr;
   PieVectTable.CLA1_INT7  = &cla1_task7_isr;
   PieVectTable.CLA1_INT8  = &cla1_task8_isr;
	EDIS;

   //Copy over the CLA code(if running in standalone mode from FLASH)
   //memcpy(&Cla1funcsRunStart, &Cla1funcsLoadStart, (Uint32)&Cla1funcsLoadSize);
   //Copy over the CLA math tables(if running in standalone mode from FLASH 
   //and using the CLAMath Library)
   //memcpy(&Cla1mathTablesRunStart, &Cla1mathTablesLoadStart, (Uint32)&Cla1mathTablesLoadSize);
	
   /*  Compute all CLA task vectors */ 
    EALLOW;
    Cla1Regs.MVECT1 = (Uint16)((Uint32)&Cla1Task1 -(Uint32)&Cla1Prog_Start);
    Cla1Regs.MVECT2 = (Uint16)((Uint32)&Cla1Task2 -(Uint32)&Cla1Prog_Start);
    Cla1Regs.MVECT3 = (Uint16)((Uint32)&Cla1Task3 -(Uint32)&Cla1Prog_Start);
    Cla1Regs.MVECT4 = (Uint16)((Uint32)&Cla1Task4 -(Uint32)&Cla1Prog_Start);
    Cla1Regs.MVECT5 = (Uint16)((Uint32)&Cla1Task5 -(Uint32)&Cla1Prog_Start);
    Cla1Regs.MVECT6 = (Uint16)((Uint32)&Cla1Task6 -(Uint32)&Cla1Prog_Start);
    Cla1Regs.MVECT7 = (Uint16)((Uint32)&Cla1Task7 -(Uint32)&Cla1Prog_Start);
    Cla1Regs.MVECT8 = (Uint16)((Uint32)&Cla1Task8 -(Uint32)&Cla1Prog_Start);
    EDIS;
   
    //  Step 3 : Mapping CLA tasks
    /*  All tasks are enabled and will be started by an ePWM trigger
     *  Map CLA program memory to the CLA and enable software breakpoints
     */
    EALLOW;
	Cla1Regs.MPISRCSEL1.bit.PERINT1SEL 	= CLA_INT1_NONE;
    Cla1Regs.MPISRCSEL1.bit.PERINT2SEL 	= CLA_INT2_NONE;
    Cla1Regs.MPISRCSEL1.bit.PERINT3SEL 	= CLA_INT3_NONE;
    Cla1Regs.MPISRCSEL1.bit.PERINT4SEL 	= CLA_INT4_NONE;
    Cla1Regs.MPISRCSEL1.bit.PERINT5SEL 	= CLA_INT5_NONE;
    Cla1Regs.MPISRCSEL1.bit.PERINT6SEL 	= CLA_INT6_NONE;
    Cla1Regs.MPISRCSEL1.bit.PERINT7SEL 	= CLA_INT7_NONE;
    Cla1Regs.MPISRCSEL1.bit.PERINT8SEL 	= CLA_INT8_NONE;
	Cla1Regs.MIER.all 		 		    = 0x00FF;
	EDIS;	
    
 
    /*  Enable CLA interrupts at the group and subgroup levels */
    PieCtrlRegs.PIEIER11.all       = 0xFFFF;
    IER = (M_INT11 );
    EINT;   // Enable Global interrupt INTM
    ERTM;   // Enable Global realtime interrupt DBGM

    /* Switch the CLA program space to the CLA and enable software forcing
     * Also switch over CLA data ram 0,1 and 2
	 * CAUTION: The RAMxCPUE bits can only be enabled by writing to the register
	 * and not the individual bit field. Furthermore, the status of these bitfields
	 * is not reflected in either the watch or register views - they always read as 
	 * zeros. This is a known bug and the user is advised to test CPU accessibilty
	 * first before proceeding
   	 */
	EALLOW;
	Cla1Regs.MMEMCFG.all = CLA_PROG_ENABLE|CLARAM0_ENABLE|CLARAM1_ENABLE|CLARAM2_ENABLE|CLA_RAM1CPUE;
	Cla1Regs.MCTL.bit.IACKE = 1;
	EDIS;	

    // Invoke Task(s)
    test_run();

    // Report Results
    test_report();

#if HARDWARE
{__asm(" ESTOP0");}
#endif
}

//###########################################################################
// CLA ISRs
//###########################################################################
__interrupt void cla1_task1_isr( void)
{
        PieCtrlRegs.PIEACK.bit.ACK11 = 1;
}

__interrupt void cla1_task2_isr( void)
{
        PieCtrlRegs.PIEACK.bit.ACK11 = 1;
}

__interrupt void cla1_task3_isr( void)
{
        PieCtrlRegs.PIEACK.bit.ACK11 = 1;
}

__interrupt void cla1_task4_isr( void)
{
        PieCtrlRegs.PIEACK.bit.ACK11 = 1;
}

__interrupt void cla1_task5_isr( void)
{
        PieCtrlRegs.PIEACK.bit.ACK11 = 1;
}

__interrupt void cla1_task6_isr( void)
{
        PieCtrlRegs.PIEACK.bit.ACK11 = 1;
}

__interrupt void cla1_task7_isr( void)
{
        PieCtrlRegs.PIEACK.bit.ACK11 = 1;
}

__interrupt void cla1_task8_isr( void)
{
        PieCtrlRegs.PIEACK.bit.ACK11 = 1;
}

