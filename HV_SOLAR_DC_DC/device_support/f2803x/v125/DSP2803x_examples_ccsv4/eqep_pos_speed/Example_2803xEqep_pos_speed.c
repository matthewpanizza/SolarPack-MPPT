//###########################################################################
// Description:
//! \addtogroup f2803x_example_list
//! <h1>eQEP Speed and Position measurement (eqep_pos_speed)</h1>
//!
//! This example provides position measurement,speed measurement using the 
//! capture unit, and speed measurement using unit time out. This example 
//! uses the IQMath library. It is used merely to simplify high-precision
//! calculations.   
//! The example requires the following hardware connections from EPWM1 and 
//! GPIO pins (simulating QEP sensor) to QEP peripheral.          
//!    - eQEP1A <- ePWM1A (simulates eQEP Phase A signal) 
//!    - eQEP1B <- ePWM1B (simulates eQEP Phase B signal)
//!    - eQEP1I <- GPIO4 (simulates eQEP Index Signal)
//! See DESCRIPTION in Example_posspeed.c for more details on the calculations
//! performed in this example.
//! In addition to this file, the following files must be included in this project:
//!    - Example_posspeed.c - includes all eQEP functions
//!    - Example_EPwmSetup.c - sets up ePWM1A and ePWM1B as simulated QA and QB
//!                            encoder signals 
//!    - Example_posspeed.h - includes initialization values for pos and speed structure
//! 
//! Note:
//!    - Maximum speed is configured to 6000rpm(BaseRpm)
//!    - Minimum speed is assumed at 10rpm for capture pre-scalar selection
//!    - Pole pair is configured to 2 (pole_pairs)
//!    - QEP Encoder resolution is configured to 4000counts/revolution (mech_scaler)
//!    - which means: 4000/4 = 1000 line/revolution quadrature encoder (simulated by EPWM1)
//!    - EPWM1 (simulating QEP encoder signals) is configured for 5kHz frequency or 300 rpm 
//!      (=4*5000 cnts/sec * 60 sec/min)/4000 cnts/rev)
//!    - SPEEDRPM_FR: High Speed Measurement is obtained by counting the QEP input pulses
//!                   for 10ms (unit timer set to 100Hz). 
//!    - SPEEDRPM_FR = (Position Delta/10ms) * 60 rpm 
//!    - SPEEDRPM_PR: Low Speed Measurement is obtained by measuring time period of QEP edges.
//!                   Time measurement is averaged over 64edges for better results and 
//!                   capture unit performs the time measurement using pre-scaled SYSCLK
//!    - pre-scaler for capture unit clock is selected such that capture timer does not 
//!      overflow at the required minimum RPM speed.
//!
//! \b External \b Connections \n
//!   - Connect eQEP1A(GPIO20) to ePWM1A(GPIO0)(simulates eQEP Phase A signal) 
//!   - Connect eQEP1B(GPIO21) to ePWM1B(GPIO1)(simulates eQEP Phase B signal)
//!   - Connect eQEP1I(GPIO23) to GPIO4 (simulates eQEP Index Signal)
//!
//! \b Watch \b Variables \n
//!  - qep_posspeed.SpeedRpm_fr - Speed meas. in rpm using QEP position counter
//!  - qep_posspeed.SpeedRpm_pr - Speed meas. in rpm using capture unit
//!  - qep_posspeed.theta_mech  - Motor mechanical angle (Q15)
//!  - qep_posspeed.theta_elec  - Motor electrical angle (Q15)
//
//###########################################################################
// $TI Release: F2803x C/C++ Header Files and Peripheral Examples V1.25 $
// $Release Date: August 26, 2011 $
//###########################################################################

#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File
#include "Example_posspeed.h"   // Example specific Include file

void initEpwm();
interrupt void prdTick(void);

POSSPEED qep_posspeed=POSSPEED_DEFAULTS;
Uint16 Interrupt_Count = 0;

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

// For this case only init GPIO for eQEP1 and ePWM1
// This function is found in DSP2803x_EQep.c
   InitEQep1Gpio();
   InitEPwm1Gpio();
   EALLOW;
   GpioCtrlRegs.GPADIR.bit.GPIO4 = 1;    // GPIO4 as output simulates Index signal
   GpioDataRegs.GPACLEAR.bit.GPIO4 = 1;  // Normally low  
   EDIS;
// Step 3. Clear all interrupts and initialize PIE vector table:
// Disable CPU interrupts 
   DINT;

// Initialize the PIE control registers to their default state.
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

// Interrupts that are used in this example are re-mapped to
// ISR functions found within this file.  
   EALLOW;  // This is needed to write to EALLOW protected registers
   PieVectTable.EPWM1_INT= &prdTick;
   EDIS;    // This is needed to disable write to EALLOW protected registers

// Step 4. Initialize all the Device Peripherals:
   initEpwm();  // This function exists in Example_EPwmSetup.c
   
// Step 5. User specific code, enable interrupts:
// Enable CPU INT1 which is connected to CPU-Timer 0:
   IER |= M_INT3;

// Enable TINT0 in the PIE: Group 3 interrupt 1
   PieCtrlRegs.PIEIER3.bit.INTx1 = 1;
   
// Enable global Interrupts and higher priority real-time debug events:
   EINT;   // Enable Global interrupt INTM
   ERTM;   // Enable Global realtime interrupt DBGM
   
   qep_posspeed.init(&qep_posspeed);
  	
	for(;;)
	{
	}

} 

interrupt void prdTick(void)                  // EPWM1 Interrupts once every 4 QCLK counts (one period)
{  Uint16 i;
   // Position and Speed measurement
   qep_posspeed.calc(&qep_posspeed);
   
   // Control loop code for position control & Speed contol
   Interrupt_Count++;
   if (Interrupt_Count==1000)                 // Every 1000 interrupts(4000 QCLK counts or 1 rev.)
   {   
       EALLOW;
       GpioDataRegs.GPASET.bit.GPIO4 = 1;     // Pulse Index signal  (1 pulse/rev.)
       for (i=0; i<700; i++){                
       }
       GpioDataRegs.GPACLEAR.bit.GPIO4 = 1;
	   Interrupt_Count = 0;                   // Reset count
	   EDIS;
   }

   // Acknowledge this interrupt to receive more interrupts from group 1
   PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
   EPwm1Regs.ETCLR.bit.INT=1;
}


