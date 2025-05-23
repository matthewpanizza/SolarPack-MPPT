//###########################################################################
//
//!  \addtogroup f2803x_example_list
//!  <h1>HRCAP Non-High Resolution Capture PWM Pulses (hrcap_capture_pwm)</h1>
//!
//!  HRCAP1 is configured for non high-resolution capture mode to 
//!  capture the time between rising and falling edges of the PWM1A output.
//!
//!  This example configures ePWM1A for:
//!  - Up count
//!  - Period starts at 80 and goes up to 4000 then back down again.
//!  - Toggle output on PRD
//!
//!  \note
//! - On Piccolo-B, the HCCAPCLK frequency is 2 * SYSCLK.EPWM period in up-count 
//!  mode is TBPRD + 1 SYSCLK counts. Therefore,\n
//!  \f$ePWM\ period(up\ count\ mode) =   2* (TBPRD+1) HCCAPCLK\ counts \f$ \n
//! - Because there is no EMU FREE/STOP support in the HRCAP peripheral,
//!  the HRCAP results cannot easily be seen in "Real-time continuous refresh"
//!  debug mode.  The only way to see accurate results of the capture is to
//!  set a breakpoint in the HRCAP ISR immediately after data has been captured.
//!  Otherwise the results read at any other time in the program via the debugger 
//!  could be mid-capture between one period and the next, and therefore bogus.
//!
//!  \b External \b Connections \n
//!  - HRCAP1 is on GPIO26
//!  - ePWM1A is on GPIO0
//!  - Connect output of ePWM1A to input of HRCAP1 (GPIO0->GPIO26)
//!
//!  \b Watch  \b Variables \n
//!  - PULSELOW
//!    - Pulse Width of Low Pulses (# of HCCAPCLK cycles - integer)
//!  - PULSEHIGH
//!    - Pulse Width of High Pulses (# of HCCAPCLK cycles - integer)
//
//###########################################################################
// $TI Release: F2803x C/C++ Header Files and Peripheral Examples V1.25 $
// $Release Date: August 26, 2011 $
//###########################################################################

#include "DSP2803x_Device.h"         // DSP2803x Headerfile
#include "DSP2803x_Examples.h"       // DSP2803x Examples Headerfile
#include "DSP2803x_EPwm_defines.h" 	 // useful defines for initialization


#define HCCAPCLK_FREQ               120                                       // HCCAPCLK Freq in MHz
#define HRCAP_SYSCLK_FREQ           60                                        // Frequency of SYSCLK sourced from same PLL as HRCAP
#define NUM_HCCAPCLKS_PER_HCSYSCLK  HCCAPCLK_FREQ/HRCAP_SYSCLK_FREQ           // # HCCAPCLK's in 1 SYSCLK (sourced from same PLL as HRCAP)
#define SYSCLK_FREQ                 60                                        // System SYSCLK Freq in MHz 


#define PWM1_TIMER_MIN 200
#define PWM1_TIMER_MAX 4000

void HRCAP1_Config(void);
void ePWM1_Config(Uint16 period);
void Fail(void);
interrupt void HRCAP1_Isr (void);

// Global variables used in program

Uint16 first;             // Count and Dly captured by 1st RISE/FALL event after cal, soft reset, or clock enable
                          // is invalid and therefore ignored. (equals # cycles from reset, cal, clock enable to edge
						  // instead of valid pulse width)
Uint16 PULSELOW; 
Uint16 PULSEHIGH; 
Uint16 EPwm1TimerDirection;
// To keep track of which way the timer value is moving
#define EPWM_TIMER_UP   1
#define EPWM_TIMER_DOWN 0


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


// These functions are in the DSP2803x_HRCap.c and DSP2803x_EPwm.c files
// respectively
   InitHRCapGpio();
   InitEPwm1Gpio();

// Step 3. Clear all interrupts and initialize PIE vector table:
// Disable CPU interrupts
  // Disable and clear all CPU interrupts
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

   EALLOW;	// This is needed to write to EALLOW protected registers
   PieVectTable.HRCAP1_INT = &HRCAP1_Isr;
   EDIS;   // This is needed to disable write to EALLOW protected registers


// Step 4. Initialize all the Device Peripherals:
// This function is found in DSP2803x_InitPeripherals.c
// InitPeripherals();  // Not required for this example
    HRCAP1_Config();                       // Configure HRCAP1 Module
    ePWM1_Config(1000);	                   // EPWM1 output waveform, Period = 500

// For this example, only initialize the EPwm
// Step 5. User specific code, enable interrupts:

// Initialize variables
   first = 0;

// Enable interrupts required for this example
   PieCtrlRegs.PIECTRL.bit.ENPIE = 1;   // Enable the PIE block
   PieCtrlRegs.PIEIER4.bit.INTx7=1;     // Enable PIE Group 4, INT 7
   IER|=M_INT4;                         // Enable CPU INT4
   ERTM;
   EINT;                                // Enable Global Interrupts

// Step 6. IDLE loop. Just sit and loop forever (optional):
   for(;;)
   {
       asm("          NOP");  // Set breakpoint here to debug
   }

}

void ePWM1_Config(Uint16 period)
{
   EALLOW;
   SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;
   EDIS;

   EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP; // Count up
   EPwm1Regs.TBPRD = PWM1_TIMER_MIN;
   EPwm1Regs.TBPHS.all = 0x00000000;
   EPwm1Regs.AQCTLA.bit.PRD = AQ_TOGGLE;      // Toggle on PRD
   
   // TBCLK = SYSCLKOUT
   EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1; 
   EPwm1Regs.TBCTL.bit.CLKDIV = TB_DIV1;

   EPwm1TimerDirection = EPWM_TIMER_UP; 
   
   EALLOW;
   SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;
   EDIS;

}


void HRCAP1_Config(void)
{
    EALLOW;
    HRCap1Regs.HCCTL.bit.SOFTRESET = 1;
	HRCap1Regs.HCCTL.bit.HCCAPCLKSEL = 1;  // HCCAPCLK = PLLCLK = SYSCLK x 2
	HRCap1Regs.HCCTL.bit.RISEINTE = 1;     // Enable Rising Edge Capture Event Interrupt
	HRCap1Regs.HCCTL.bit.FALLINTE = 0;	   // Disable Falling Edge Capture Event Interrupt
	HRCap1Regs.HCCTL.bit.OVFINTE = 0;	   // Disable Interrupt on 16-bit Counter Overflow Event
}

interrupt void HRCAP1_Isr (void)
{
    EALLOW;
	if (HRCap1Regs.HCIFR.bit.RISEOVF == 1) {
		ESTOP0;                    // Another rising edge detected before ISR serviced.
   }

	if (first==0) {
	    first = 1;  // Discard first data (because first interrupt after reset/clk enable
		            // measures time from clock start to edge instead of valid pulse width)
	} else {

	    HRCap1Regs.HCCTL.bit.RISEINTE = 0; // Disable rising edge interrupts

       // Current Low and High Pulse
	   // In up-count mode: ePWM pulse width = TBPRD + 1 SYSCLK cycles  
       //                                    = (TBPRD + 1) * (HCCAPCLK_FREQ/SYSCLK_FREQ) HCCAPCLK cycles
	   //  Variance = +/- 1 SYSCLK cycles (or +/- #HCCAPCLK's/SYSCLK).
	   //
	   //  The extra +/- 1 HCCAPCLK cycle check in this example is to account for EPWM_TIMER_MAX and EPWM_TIMER_MIN
	   //  limits when switching between UP_COUNT/DOWN_COUNT, and pulse captured is still the last period instead of 
	   //  current period.
		
        // Capture current high pulse width in HCCAPCLK cycles
		PULSEHIGH = HRCap1Regs.HCCAPCNTFALL1 + 1; 
		
	    if ((PULSEHIGH > ((Uint32)(EPwm1Regs.TBPRD + 1)* HCCAPCLK_FREQ/SYSCLK_FREQ + NUM_HCCAPCLKS_PER_HCSYSCLK + 1))||
	        (PULSEHIGH < ((Uint32)(EPwm1Regs.TBPRD + 1)* HCCAPCLK_FREQ/SYSCLK_FREQ - NUM_HCCAPCLKS_PER_HCSYSCLK - 1))) 
	    {
		    ESTOP0;
		} 
		
        // Capture low pulse width immediately preceding high pulse width in HCCAPCLK cycles
		PULSELOW = HRCap1Regs.HCCAPCNTRISE0 + 1; 
	    if ((PULSELOW > ((Uint32)(EPwm1Regs.TBPRD + 1) * HCCAPCLK_FREQ/SYSCLK_FREQ + NUM_HCCAPCLKS_PER_HCSYSCLK + 1))||
	        (PULSELOW < ((Uint32)(EPwm1Regs.TBPRD + 1) * HCCAPCLK_FREQ/SYSCLK_FREQ - NUM_HCCAPCLKS_PER_HCSYSCLK - 1))) {
		    ESTOP0;  
		}		
	    
	    // Modulate EPWM1 TBPRD register between TIMER_MAX and TIMER_MIN
	    if (EPwm1TimerDirection == EPWM_TIMER_UP)
        {
            if(EPwm1Regs.TBPRD < PWM1_TIMER_MAX) 
            {
               EPwm1Regs.TBPRD++;
            }
            else
            {
               EPwm1TimerDirection = EPWM_TIMER_DOWN;
               EPwm1Regs.TBPRD--;
            }
        }
        else
        {
            if(EPwm1Regs.TBPRD > PWM1_TIMER_MIN) 
            {
               EPwm1Regs.TBPRD--;
            }
            else
            {
               EPwm1TimerDirection = EPWM_TIMER_UP;
               EPwm1Regs.TBPRD++;
            }
        }  
 
   } 
   
    HRCap1Regs.HCICLR.all = 0x001F;          // Clear all HRCAP interrupts
    HRCap1Regs.HCCTL.bit.RISEINTE = 1;       // Re-enable rising edge interrupts      

    HRCap1Regs.HCICLR.bit.INT=1;   // Clear HRCAP interrupt flag
	PieCtrlRegs.PIEACK.bit.ACK4=1; // Acknowledge PIE Group 4 interrupts.
    EDIS;
}




