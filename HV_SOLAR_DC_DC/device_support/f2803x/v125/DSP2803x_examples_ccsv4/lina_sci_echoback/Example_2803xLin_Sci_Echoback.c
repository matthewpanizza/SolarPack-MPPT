//###########################################################################
//
//!  \addtogroup f2803x_example_list
//!  <h1>LIN-SCI Digital Loop Back(lina_sci_echoback)</h1>
//!
//!  This test receives and echos back data through the LIN-A port 
//!  which has been configured for SCI operation.  
//!
//!  The PC application 'hypterterminal' can be used to view the data
//!  from the LIN and to send information to the LIN.  Characters received
//!  by the LIN port are sent back to the host.
//!
//!  \b Running \b the \b Application   
//!  -# Configure hyperterminal:
//!  Use the included hyperterminal configuration file SCI_96.ht.
//!  To load this configuration in hyperterminal
//!    -# Open hyperterminal
//!    -# Go to file->open
//!    -# Browse to the location of the project and
//!       select the SCI_96.ht file.
//!  -# Check the COM port.
//!  The configuration file is currently setup for COM1.
//!  If this is not correct, disconnect (Call->Disconnect)
//!  Open the File-Properties dialog and select the correct COM port.
//!  -# Connect hyperterminal Call->Call
//!  and then start the 2803x LIN-SCI echoback program execution.
//!  -# The program will print out a greeting and then ask you to
//!  enter a character which it will echo back to hyperterminal.
//!
//!  \note If you are unable to open the .ht file, you can create 
//!  a new one with the following settings
//!  -  Find correct COM port
//!  -  Bits per second = 19200
//!  -  Date Bits = 8
//!  -  Parity = None
//!  -  Stop Bits = 1
//!  -  Hardware Control = None
//!
//!  \b Watch \b Variables \n
//!  - \b LoopCount, for the number of characters sent
//!  - \b ReceivedChar, for character received from Hyperterminal
//!
//! \b External \b Connections \n
//!  Connect the LIN-A port to a PC via a transceiver and cable.
//!  - GPIO9 is LIN-TX
//!  - GPIO11 is LIN-RX
//
//###########################################################################
// $TI Release: F2803x C/C++ Header Files and Peripheral Examples V1.25 $
// $Release Date: August 26, 2011 $
//###########################################################################

#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File

// Prototype statements for functions found within this file.
void SetupSCI(void);
void error(void);
void scia_xmit(char Char);
void scia_msg(char *msg);

// Global variables
Uint16 LoopCount;
Uint16 ReceivedChar;


void main(void)
{
	//Initialize Variables

	//Step 1. Initialize System Control:
	//PLL, WatchDog, enable Peripheral Clocks
	//This example function is found in the DSP2803x_SysCtrl.c file.
   	InitSysCtrl();

   	EALLOW;

	//Step 2. Initalize GPIO:
	//This example function is found in the DSP2803x_Gpio.c file and
	//illustrates how to set the GPIO to it's default state.
	//InitGpio();
	//Setup only the GP I/O only for SCI-A and SCI-B functionality
	//This function is found in DSP2803x_Sci.c
    InitLinGpio();

	//Step 3. Clear all interrupts and initialize PIE vector table:
	//Disable CPU interrupts
   	DINT;

	//Initialize PIE control registers to their default state.
	//The default state is all PIE interrupts disabled and flags
	//are cleared.
	//This function is found in the DSP2803x_PieCtrl.c file.
   	InitPieCtrl();

	//Disable CPU interrupts and clear all CPU interrupt flags:
   	IER = 0x0000;
   	IFR = 0x0000;

    //Initialize and Enable BLIN SCI module
    SetupSCI();

	//Step 4. Initialize all the Device Peripherals:
	//This function is found in DSP2803x_InitPeripherals.c
	//InitPeripherals(); // Not required for this example

	//Step 5. User specific code, enable interrupts:

	//Init send data.  After each transmission this data
	//will be updated for the next transmission

   	EALLOW;

	LoopCount = 0;

	//Wait for SCI to be idle and ready for transmission
	while(LinaRegs.SCIFLR.bit.IDLE == 1);

    scia_msg("\r\nYou will enter a character, and the DSP will echo it back! \n\0");
    for(;;)
    {
    	scia_msg("\r\nEnter a character: \0");

		//Wait for a charachter to by typed
		while(LinaRegs.SCIFLR.bit.RXRDY == 0);

		ReceivedChar = LinaRegs.SCIRD;

		scia_msg("  You sent: \0");
		scia_xmit(ReceivedChar);

		LoopCount++;
    }
}

void scia_xmit(char Char)
{
	//Wait for the module to be ready to transmit
	while(LinaRegs.SCIFLR.bit.TXRDY == 0);

	//Begin transmission
	LinaRegs.SCITD = Char;
}

void scia_msg(char *msg)
{
	int it;
    it = 0;
    while(msg[it] != '\0')
    {
        scia_xmit(msg[it]);
        it++;
    }
}

void SetupSCI(void)
{
	//Allow write to protected registers
	EALLOW;

	LinaRegs.SCIGCR0.bit.RESET = 0; //Into reset
	LinaRegs.SCIGCR0.bit.RESET = 1; //Out of reset

	LinaRegs.SCIGCR1.bit.SWnRST = 0; //Into software reset

	//SCI Configurations
	LinaRegs.SCIGCR1.bit.COMMMODE = 0;   //Idle-Line Mode
	LinaRegs.SCIGCR1.bit.TIMINGMODE = 1; //Asynchronous Timing
	LinaRegs.SCIGCR1.bit.PARITYENA = 0;  //No Parity Check
	LinaRegs.SCIGCR1.bit.PARITY = 0;	 //Odd Parity
	LinaRegs.SCIGCR1.bit.STOP = 0;		 //One Stop Bit
	LinaRegs.SCIGCR1.bit.CLK_MASTER = 1; //Enable SCI Clock
	LinaRegs.SCIGCR1.bit.LINMODE = 0;	 //SCI Mode
	LinaRegs.SCIGCR1.bit.SLEEP = 0;      //Ensure Out of Sleep
	LinaRegs.SCIGCR1.bit.MBUFMODE = 0;	 //No Buffers Mode
	LinaRegs.SCIGCR1.bit.LOOPBACK = 0;   //External Loopback
	LinaRegs.SCIGCR1.bit.CONT = 1;		 //Continue on Suspend
	LinaRegs.SCIGCR1.bit.RXENA = 1;		 //Enable RX
	LinaRegs.SCIGCR1.bit.TXENA = 1;		 //Enable TX

	//Ensure IODFT is disabled
    LinaRegs.IODFTCTRL.bit.IODFTENA = 0x0;

    //Set transmission length
    LinaRegs.SCIFORMAT.bit.CHAR = 7;	 //Eight bits
    LinaRegs.SCIFORMAT.bit.LENGTH = 0;   //One byte

	//Set baudrate
    LinaRegs.BRSR.bit.SCI_LIN_PSL = 194;          //Baud = 9.6khz
    LinaRegs.BRSR.bit.M = 5;

    LinaRegs.SCIGCR1.bit.SWnRST = 1;  //bring out of software reset

	//Disable write to protected registers
	EDIS;
}

void error(void)
{
	asm("     ESTOP0"); // Test failed!! Stop!
	for (;;);
}

//===========================================================================
// No more.
//===========================================================================



