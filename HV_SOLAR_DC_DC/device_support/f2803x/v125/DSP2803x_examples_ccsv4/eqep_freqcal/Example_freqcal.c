//###########################################################################
//
//!  \addtogroup f2803x_example_list
//!  \section EXAMPLE_FREQCAL_C Frequency Calculation (Example_freqcal.c)
//!
//!  This file includes the EQEP initialization and frequency calculation
//!  functions called by \b Example_2803xEqep_freqcal.c.  The frequency calculation
//!  steps performed by FREQCAL_Calc()at  SYSCLKOUT = 60 MHz are
//!  described below:
//!
//!  -# This program calculates: \b **freqhz_fr** \n
//!  \f[ freqhz\_fr\ or\ v = \frac{x_{2}-x_{1}}{T} .......... 1 \f] \n
//!  If \f[ \frac{max}{base}\ freq = 10kHz => 10kHz = \frac{x_{2}-x_{1}}{(2/100Hz)} .......... 2\f] \n
//!  \f[ max (x_{2}-x_{1}) = 200 counts = freqScaler\_fr \f]
//!  
//!  \note \f$ T = \frac{2}{100Hz} \f$ . 2 is from \f$ \frac{x_{2}-x_{1}}{2}\f$ 
//!  because QPOSCNT counts 2 edges per cycle (rising and falling)
//!
//!  If both sides of Equation 2 are divided by 10 kHz, then: \n
//!  \f[ 1 = \frac{x_{2}-x_{1}}{10kHz*(2/100Hz)}\f]\n where, \f[ [10kHz*\frac{2}{100Hz}] = 200 \f]\n
//!  Because \f[ x_{2}-x_{1} < 200 (max)\f]
//!  \f[ \frac{x_{2}-x_{1}}{200} < 1 \f]\n for all frequencies less than max\n
//!  \f[ freq\_fr = \frac{x_{2}-x_{1}}{200}\ or\ \frac{x_{2}-x_{1}}{10kHz*(2/100Hz)} .......... 3\f] \n
//!  
//!  To get back to original velocity equation, Equation 1, multiply Equation 3 by 10 kHz\n
//!  \f[ freqhz\_fr(or\ velocity) = 10kHz*\frac{x_{2}-x_{1}}{10kHz*(2/100Hz)}\f]\n
//!  \f[ = \frac{x_{2}-x_{1}}{(2/100Hz)} .......... final\ equation\f] \n
//!
//!  -# \b **min \b freq** \f$ = \frac{1\ count}{(2/100Hz)} = 50 Hz\f$
//!  -# \b **freqhz_pr** \n
//!  \f[ freqhz\_pr\ or\ v = \frac{X}{t_{2}-t_{1}} .......... 4\f] \n
//!  If \f[ \frac{max}{base}\ freq = 10kHz => 10kHz = \frac{(8/2)}{T} = \frac{8}{2T} \f] \n
//!  where,
//!    - 8 = QCAPCTL [UPPS] (Unit timeout - once every 8 edges)
//!    - 2 = divide by 2 because QPOSCNT counts 2 edges per cycle (rising and falling)
//!    - T = time in seconds 
//!        = \f$ \frac{t_{2}-t_{1}}{(100MHz/128)}, t_{2}-t_{1} \f$ = # of QCAPCLK cycles,
//!        \n and \f$ 1\ QCAPCLK\ cycle = \frac{1}{(100MHz/128)} = QCPRDLAT \f$
//!
//!  So: \f[ 10 kHz = 8*\frac{(60MHz/128)}{2*(t_{2}-t_{1})} \f] \n
//!  \f[ t_{2}-t_{1} = 8*\frac{(60MHz/128)}{10kHz*2} = \frac{(60MHz/128)}{((2*10KHz)/8)} .......... 5\f] \n
//!  \f[ = 188\ QCAPCLK\ cycles = maximum (t_{2}-t_{1})  = freqScaler\_pr \f]
//!  Divide both sides by \f$ (t_{2}-t_{1}) \f$, and:\n
//!  \f[ 1 = \frac{188}{t_{2}-t_{1}} = \frac{(60MHz/128)/((2*10KHz)/8)}{t_{2}-t_{1}} \f]\n
//!  Because \f$ (t_{2}-t_{1}) < 188 (max) \f$,
//!  \f$ \frac{188}{t_{2}-t_{1}} < 1 \f$ for all frequencies less than max \n
//!  \f[ freq\_pr = \frac{188}{t_{2}-t_{1}}\ or\ \frac{(60MHz/128)/((2*10KHz)/8)}{t_{2}-t_{1}} .......... 6\f] \n
//!  Now within velocity limits, to get back to original velocity equation, Equation 1,
//!  multiply Equation 6 by 10 kHz: \n
//!  \f[ freqhz\_fr(or\ velocity) = 10kHz*\frac{(60MHz/128)/((2*10KHz)/8)}{t_{2}-t_{1}}\f]\n
//!  \f[ = \frac{(60MHz/128)*8}{2*(t_{2}-t_{1})} \f]\n
//!  or \f[ \frac{8}{2*(t_{2}-t_{1})*(QCPRDLAT)} .......... final\ equation\f] \n
//!
//!  More detailed calculation results can be found in the Example_freqcal.xls
//!  spreadsheet included in the example folder.
//!
//
//###########################################################################
// $TI Release: F2803x C/C++ Header Files and Peripheral Examples V1.25 $
// $Release Date: August 26, 2011 $
//###########################################################################

#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File
#include "Example_freqcal.h"    // Example specific include file

void  FREQCAL_Init(void)
{

	EQep1Regs.QUPRD=600000;			// Unit Timer for 100Hz at 60 MHz SYSCLKOUT

	EQep1Regs.QDECCTL.bit.QSRC=2;		// Up count mode (freq. measurement)
	EQep1Regs.QDECCTL.bit.XCR=0;        // 2x resolution (cnt falling and rising edges)

	EQep1Regs.QEPCTL.bit.FREE_SOFT=2;
	EQep1Regs.QEPCTL.bit.PCRM=00;		// QPOSCNT reset on index evnt
	EQep1Regs.QEPCTL.bit.UTE=1; 		// Unit Timer Enable
	EQep1Regs.QEPCTL.bit.QCLM=1; 		// Latch on unit time out
	EQep1Regs.QPOSMAX=0xffffffff;
	EQep1Regs.QEPCTL.bit.QPEN=1; 		// QEP enable

	EQep1Regs.QCAPCTL.bit.UPPS=3;   	// 1/8 for unit position
	EQep1Regs.QCAPCTL.bit.CCPS=7;		// 1/128 for CAP clock
	EQep1Regs.QCAPCTL.bit.CEN=1; 		// QEP Capture Enable

}

void FREQCAL_Calc(FREQCAL *p)
{
     unsigned long tmp;
   	 _iq newp,oldp;

// Check unit Time out-event for speed calculation:
// Unit Timer is configured for 100Hz in INIT function
//**** Freq Calcultation using QEP position counter ****//
// For a more detailed explanation of the calculation, read
// the description at the top of this file

	if(EQep1Regs.QFLG.bit.UTO==1)                  // Unit Timeout event
	{
		/** Differentiator	**/
	 	newp=EQep1Regs.QPOSLAT;                    // Latched POSCNT value
		oldp=p->oldpos;

    	if (newp>oldp)
      		tmp = newp - oldp;                     // x2-x1 in v=(x2-x1)/T equation
    	else
      		tmp = (0xFFFFFFFF-oldp)+newp;

		p->freq_fr = _IQdiv(tmp,p->freqScaler_fr); // p->freq_fr = (x2-x1)/(T*10KHz)
		tmp=p->freq_fr;

		if (tmp>=_IQ(1))          // is freq greater than max freq (10KHz for this example)?
	 		p->freq_fr = _IQ(1);
		else
	 		p->freq_fr = tmp;

		p->freqhz_fr = _IQmpy(p->BaseFreq,p->freq_fr); 	// Q0 = Q0*GLOBAL_Q => _IQXmpy(), X = GLOBAL_Q
		                                                // p->freqhz_fr = (p->freq_fr)*10kHz = (x2-x1)/T

		// Update position counter
    	p->oldpos = newp;
		//=======================================

		EQep1Regs.QCLR.bit.UTO=1;					// Clear interrupt flag
	}

//**** Freq Calcultation using QEP capture counter ****//
	if(EQep1Regs.QEPSTS.bit.UPEVNT==1)              // Unit Position Event
	{
		if(EQep1Regs.QEPSTS.bit.COEF==0)            // No Capture overflow
			tmp=(unsigned long)EQep1Regs.QCPRDLAT;
		else							            // Capture overflow, saturate the result
			tmp=0xFFFF;

		p->freq_pr = _IQdiv(p->freqScaler_pr,tmp);  // p->freq_pr = X/[(t2-t1)*10KHz]
		tmp=p->freq_pr;

		if (tmp>_IQ(1))
	 		p->freq_pr = _IQ(1);
		else
	 		p->freq_pr = tmp;

		p->freqhz_pr = _IQmpy(p->BaseFreq,p->freq_pr); 	// Q0 = Q0*GLOBAL_Q => _IQXmpy(), X = GLOBAL_Q
	                                                    // p->freqhz_pr =( p->freq_pr)*10kHz = X/(t2-t1)
		EQep1Regs.QEPSTS.all=0x88;					    // Clear Unit position event flag
												     	// Clear overflow error flag
	}

}


