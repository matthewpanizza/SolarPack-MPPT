//###########################################################################
//
//! \addtogroup f2803x_example_list
//! <h1>GPIO Toggle Test (gpio_toggle)</h1>
//!
//!  \note ALL OF THE I/O'S TOGGLE IN THIS PROGRAM.  MAKE SURE
//!  THIS WILL NOT DAMAGE YOUR HARDWARE BEFORE RUNNING THIS
//!  EXAMPLE.
//!
//!  Three different examples are included. Select the example 
//!  (data, set/clear or toggle) to execute before compiling using
//!  the macros found at the top of the code.   
//!
//!  Each example toggles all the GPIOs in a different way, the first
//!  through writing values to the GPIO DATA registers, the second through
//!  the SET/CLEAR registers and finally the last through the TOGGLE register
//!    
//!  The pins can be observed using Oscilloscope.  
// 
//###########################################################################
// $TI Release: F2803x C/C++ Header Files and Peripheral Examples V1.25 $
// $Release Date: August 26, 2011 $
//###########################################################################

#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File

// Select the example to compile in.  Only one example should be set as 1
// the rest should be set as 0.
#define EXAMPLE1 1  // Use DATA registers to toggle I/O's
#define EXAMPLE2 0  // Use SET/CLEAR registers to toggle I/O's
#define EXAMPLE3 0  // Use TOGGLE registers to toggle I/O's

// Prototype statements for functions found within this file.
void delay_loop(void);
void Gpio_select(void);
void Gpio_example1(void);
void Gpio_example2(void);
void Gpio_example3(void);

void main(void)
{

// Step 1. Initialize System Control:
// PLL, WatchDog, enable Peripheral Clocks
// This example function is found in the DSP2803x_SysCtrl.c file.
   InitSysCtrl();
   
// Step 2. Initalize GPIO: 
// This example function is found in the DSP2803x_Gpio.c file and
// illustrates how to set the GPIO to it's default state.
// InitGpio();  // Skipped for this example
 
// For this example use the following configuration:
   Gpio_select();	  

// Step 3. Clear all interrupts and initialize PIE vector table:
// Disable CPU interrupts 
   DINT;

// Initialize PIE control registers to their default state.
// The default state is all PIE interrupts disabled and flags
// are cleared.  
// This function is found in the DSP2803x_PieCtrl.c file.
   InitPieCtrl();

// Disable CPU interrupts and clear all CPU interrupt flags:
   IER = 0x0000;
   IFR = 0x0000;

// Initialize the PIE vector table with pointers to the shell Interrupt 
// Service Routines (ISR).  
// This will populate the entire table, even if the interrupt
// is not used in this example.  This is useful for debug purposes.
// The shell ISR routines are found in DSP2803x_DefaultIsr.c.
// This function is found in DSP2803x_PieVect.c.
   InitPieVectTable();

	
// Step 4. Initialize all the Device Peripherals:
// This function is found in DSP2803x_InitPeripherals.c
// InitPeripherals(); // Not required for this example
	
// Step 5. User specific code:
	
#if EXAMPLE1

    // This example uses DATA registers to toggle I/O's
    Gpio_example1();

#endif  // - EXAMPLE1

#if EXAMPLE2

    // This example uses SET/CLEAR registers to toggle I/O's
    Gpio_example2();
    
#endif

#if EXAMPLE3

    // This example uses TOGGLE registers to toggle I/O's
    Gpio_example3();
    
#endif

} 	

void delay_loop()
{
    short      i;
    for (i = 0; i < 1000; i++) {}
}

void Gpio_example1(void)
{ 
   // Example 1:
   // Toggle I/Os using DATA registers

   for(;;)
   {   
       GpioDataRegs.GPADAT.all    =0xAAAAAAAA;
       GpioDataRegs.GPBDAT.all    =0x0000000A;   			  
       
       delay_loop();

       GpioDataRegs.GPADAT.all    =0x55555555; 
       GpioDataRegs.GPBDAT.all    =0x00000005; 
    
       delay_loop();	
    }
}

void Gpio_example2(void)
{ 
   // Example 2:
   // Toggle I/Os using SET/CLEAR registers
   for(;;)
   {    
   
       GpioDataRegs.GPASET.all    =0xAAAAAAAA;
       GpioDataRegs.GPACLEAR.all  =0x55555555;
       
       GpioDataRegs.GPBSET.all    =0x0000000A;   			  
       GpioDataRegs.GPBCLEAR.all  =0x00000005;   			  
              
       delay_loop();

       GpioDataRegs.GPACLEAR.all    =0xAAAAAAAA;
       GpioDataRegs.GPASET.all      =0x55555555;
       
       GpioDataRegs.GPBCLEAR.all    =0x0000000A;   			  
       GpioDataRegs.GPBSET.all      =0x00000005;   			  
    
       delay_loop();	
   
    }
}

void Gpio_example3(void)
{ 
   // Example 2:
   // Toggle I/Os using TOGGLE registers

   // Set pins to a known state
   
      GpioDataRegs.GPASET.all    =0xAAAAAAAA;
      GpioDataRegs.GPACLEAR.all  =0x55555555;
       
      GpioDataRegs.GPBSET.all    =0x0000000A;   			  
      GpioDataRegs.GPBCLEAR.all  =0x00000005;   			     

   // Use TOGGLE registers to flip the state of
   // the pins. 
   // Any bit set to a 1 will flip state (toggle)
   // Any bit set to a 0 will not toggle.   

   for(;;)
   {    
      GpioDataRegs.GPATOGGLE.all =0xFFFFFFFF;
      GpioDataRegs.GPBTOGGLE.all =0x0000000F;   			  
      delay_loop();
    }
}

void Gpio_select(void)
{

   
    EALLOW;
	GpioCtrlRegs.GPAMUX1.all = 0x00000000;  // All GPIO
	GpioCtrlRegs.GPAMUX2.all = 0x00000000;  // All GPIO
	GpioCtrlRegs.GPAMUX1.all = 0x00000000;  // All GPIO
    GpioCtrlRegs.GPADIR.all = 0xFFFFFFFF;   // All outputs
    GpioCtrlRegs.GPBDIR.all = 0x0000000F;   // All outputs
    EDIS;
     
}     
//===========================================================================
// No more.
//===========================================================================


