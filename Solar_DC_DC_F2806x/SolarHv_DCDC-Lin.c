#include "DSP28x_Project.h"     // Device Headerfile and Examples Include File
#include "SolarHv_DCDC-Lin.h"

#define	PktSizeLin				2

// Prototype statements for functions found within this file.
interrupt void Lina_Level0_ISR(void);

void PackTxBuffersLin(void);
void UnpackRxBuffersLin(void);
void SetupLin(void);
void error(void);


// Global variables
Uint16 sdataALin;     // Send data for SCI-A
Uint16 rdataALin;     // Received data for SCI-A


Uint32 LinL0IntVect;  //Vector for L0 interrupts

extern Uint32 LinL0IntCount;

Uint16 iLin;			  //Generic iterator
int errorcount=0;


void (*RcvTaskPointerLin)(void); 			// State pointer for Command Packet Receive
Uint16	CmdPacketLin[PktSizeLin];


void GetCmdByteLin(void);
void EchoCmdByteLin(void);
void GetIndexByteLin(void);
void EchoIndexByteLin(void);
void GetDataByteLin(void);
void EchoDataByteLin(void);
void SendDataByteLin(void);
void CompleteByteLin(void);


int16* Gui_wPanelVolt;
int16* Gui_wPanelCurrent;
int16* Gui_wBoostOutputVolt;
int16* Gui_wPanelOutputPower;
int16* Gui_wMPPT_Status;
int16 Gui_wSysMode = 5;
int16 Gui_wLLCOutputVolt = 6;
int16 Gui_wTurnOn = 7;
int16 Gui_wTurnOff = 8;
int16 Gui_wMPPT_OnOff = 9;


int16 LinTestTemp = 0;
int16 LinTestTemp2 = 0;


//LIN
//======== SM Entry Point =================
void SerialHostCommsLin()
{		
	(*RcvTaskPointerLin)();	// Call routine pointed to by state pointer
}
//=========================================


void GetCmdByteLin(void) // Task 1
{
	UnpackRxBuffersLin();
	RcvTaskPointerLin = &EchoCmdByteLin;		// point to next state
	//VTimer0[1] = 0;
	EchoCmdByteLin();                                                                      
	/*
	if (VTimer0[1] > 2000) //2 ms timeout
	{	
		VTimer0[1] = 0;
								                                      
		//CommsOKflg = 0;
		RcvTaskPointer = &GetCmdByte;		// go back and wait for new CMD                       
	}*/
}


void EchoCmdByteLin(void) // Task 2
{
    sdataALin = rdataALin;
    PackTxBuffersLin();			// echo back the received char
    CmdPacketLin[0] = sdataALin;      // Command
    if(CmdPacketLin[0] <= 2)
    {
    	RcvTaskPointerLin = &GetIndexByteLin;
    }
    else if(CmdPacketLin[0] > 2)
    {
    	switch(CmdPacketLin[0])
    	{
    		case 3: Gui_wTurnOn = 1;
    				break;
    		case 4: Gui_wTurnOff = 1;
    				break;
    		case 5: Gui_wMPPT_OnOff = 2;
    				break;
    		case 6: Gui_wMPPT_OnOff = 1;
    				break;
    		default: 
    				break;
    	}
    	// clear Command Packet
		for (iLin=0; iLin<PktSizeLin; iLin++)
		{
			CmdPacketLin[iLin] = 0x0;
		}
    	RcvTaskPointerLin = &GetCmdByteLin;
    }    
	//RcvTaskPointer = &GetIndexByte;
	//VTimer0[1] = 0;						// Reset Time-out timer
}


void GetIndexByteLin(void) // Task 3
{
	UnpackRxBuffersLin();

	RcvTaskPointerLin = &EchoIndexByteLin;		// point to next state
		
	EchoIndexByteLin();
	/*
	if (VTimer0[1] > 500)				// 500*4mS = 2 sec timeout
	{
		//CommsOKflg = 0;
		RcvTaskPointer = &GetCmdByte;		// Abort, go back wait for new CMD
		VTimer0[1] = 0;
	}*/
}

void EchoIndexByteLin(void) // Task 4
{
    sdataALin = rdataALin;
    CmdPacketLin[1] = sdataALin;		//Index
    PackTxBuffersLin();			//echo back the received char  
    if(CmdPacketLin[0] == 1)
    {
		RcvTaskPointerLin = &SendDataByteLin;
		//SendDataByte();
    }
    else if(CmdPacketLin[0] == 2)
    {
    	RcvTaskPointerLin = &GetDataByteLin;
    }
	//VTimer0[1] = 0;						// Reset Time-out timer
}


void GetDataByteLin(void) // Task 5
{
	UnpackRxBuffersLin();
	
	if(CmdPacketLin[1] == 0)
	{
		Gui_wSysMode = rdataALin;
	}
	if(CmdPacketLin[1] == 1)
	{
		Gui_wLLCOutputVolt = rdataALin;
	}
	
	RcvTaskPointerLin = &EchoDataByteLin;		// point to next state
	EchoDataByteLin();				
	/*
	if (VTimer0[1] > 500)				// 500*4mS = 2 sec timeout
	{
		//CommsOKflg = 0;
		RcvTaskPointer = &GetCmdByte;		// Abort, go back wait for new CMD
		VTimer0[1] = 0;
	}*/	
}


void EchoDataByteLin(void) // Task 6
{
    sdataALin = 0x00ff;
    PackTxBuffersLin();			// if yes, echo back the received char
    // clear Command Packet
	for (iLin=0; iLin<PktSizeLin; iLin++)
	{
		CmdPacketLin[iLin] = 0x0;
	}
	RcvTaskPointerLin = &GetCmdByteLin;
}


void SendDataByteLin(void) // Task 7
{
	UnpackRxBuffersLin();
	if(rdataALin == CmdPacketLin[1])
	{ 
		switch(CmdPacketLin[1])
		{
			case 0: sdataALin = *Gui_wPanelVolt;
					break;
			case 1: sdataALin = *Gui_wPanelCurrent;
					break;
			case 2: sdataALin = *Gui_wBoostOutputVolt;
					break;
			case 3: sdataALin = *Gui_wPanelOutputPower;
					break;
			case 4: sdataALin = *Gui_wMPPT_Status;
					break;
			default:
					break;
		}
		PackTxBuffersLin();
		RcvTaskPointerLin = &CompleteByteLin;	
	}
	
	else
		error();				
	/*
	if (VTimer0[1] > 500)				// 500*4mS = 2 sec timeout
	{
		//CommsOKflg = 0;
		RcvTaskPointer = &GetCmdByte;		// Abort, go back wait for new CMD
		VTimer0[1] = 0;
	}*/	
}

void CompleteByteLin(void) // Task 8
{
    UnpackRxBuffersLin();
    if(rdataALin == 0x00ff)
    {
    	// clear Command Packet
		for (iLin=0; iLin<PktSizeLin; iLin++)
		{
			CmdPacketLin[iLin] = 0x0;
		}
		RcvTaskPointerLin = &GetCmdByteLin;
		rdataALin = 0;
		sdataALin = 0;
    }
    else
    	error();
    /*
    if (VTimer0[1] > 500)				// 500*4mS = 2 sec timeout
	{
		//CommsOKflg = 0;
		RcvTaskPointer = &GetCmdByte;		// Abort, go back wait for new CMD
		VTimer0[1] = 0;
	}*/
}




//High priority LIN ISR.
interrupt void Lina_Level0_ISR(void)
{
	//Increment the interrupt counter
	LinL0IntCount++;
	
	EALLOW;
	//LinaRegs.SCIFLR.bit.IDLE == 1;
	EDIS;

	//Read-clear the interrupt vector
	//LinL0IntVect = LinaRegs.SCIINTVECT0.all;

	//RXINT
	if(LinL0IntVect == 11)
	{		
		SerialHostCommsLin();
		if(LinTestTemp == 0)
		{
			//GpioDataRegs.GPADAT.bit.GPIO31 = 0;//mModeLed_ON();
			//LinTestTemp = 1;
			LinTestTemp2=300;
			LinTestTemp=500;
		}
		//else if(LinTestTemp == 1)
		else if(LinTestTemp2 == 0)
		{
			//GpioDataRegs.GPADAT.bit.GPIO31 = 1;//mModeLed_OFF();
			//LinTestTemp = 0;
			LinTestTemp=50;
			LinTestTemp2=500;
		}
		else
		{
			LinTestTemp--;
			LinTestTemp2--;
		}
	}			
	
	//Other Interrupt
	else
	{
		//There should not be other interrupts
		error();
		if(LinTestTemp == 0)
		{
			//GpioDataRegs.GPADAT.bit.GPIO31 = 0;//mModeLed_ON();
			//LinTestTemp = 1;
			LinTestTemp2=30;
			LinTestTemp=50;
		}
		//else if(LinTestTemp == 1)
		else if(LinTestTemp2 == 0)
		{
			//GpioDataRegs.GPADAT.bit.GPIO31 = 1;//mModeLed_OFF();
			//LinTestTemp = 0;
			LinTestTemp=5;
			LinTestTemp2=50;
		}
		else
		{
			LinTestTemp--;
			LinTestTemp2--;
		}
	}
	
	EALLOW;
	//LinaRegs.SCIFLR.bit.IDLE == 0;
	EDIS;

	//Acknowledge the PIE to be able to receive more interrupts
	//PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
}


//Pack array words into LINTD buffer bytes
//sending high byte first.
void PackTxBuffersLin(void)
{
	//LinaRegs.LINTD0.bit.TD0 = sdataALin >> 8;
	//LinaRegs.LINTD0.bit.TD1 = sdataALin &  0x00FF;
	/*
	LinaRegs.LINTD0.bit.TD2 = sdataA[1] >> 8;
	LinaRegs.LINTD0.bit.TD3 = sdataA[1] &  0x00FF;
	LinaRegs.LINTD1.bit.TD4 = sdataA[2] >> 8;
	LinaRegs.LINTD1.bit.TD5 = sdataA[2] &  0x00FF;
	LinaRegs.LINTD1.bit.TD6 = sdataA[3] >> 8;
	LinaRegs.LINTD1.bit.TD7 = sdataA[3] &  0x00FF;
	*/
}

//Move data from LINRD buffers to data array.
void UnpackRxBuffersLin(void)
{
	Uint32 ReadData;
	//ReadData = LinaRegs.LINRD0.all;
	rdataALin = (ReadData & 0xFFFF0000) >> 16;
	/*
	rdataA[1] = ReadData & 0x0000FFFF;
	ReadData = LinaRegs.LINRD1.all;
	rdataA[2] = (ReadData & 0xFFFF0000) >> 16;
	rdataA[3] = ReadData & 0x0000FFFF;
	*/
}

void SetupLin(void)
{
	//Allow write to protected registers
	EALLOW;

	/*LinaRegs.SCIGCR0.bit.RESET = 0; //Into reset
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
	LinaRegs.SCIGCR1.bit.MBUFMODE = 1;	 //Buffered Mode
	LinaRegs.SCIGCR1.bit.LOOPBACK = 0;   //External Loopback
	LinaRegs.SCIGCR1.bit.CONT = 1;		 //Continue on Suspend
	LinaRegs.SCIGCR1.bit.RXENA = 1;		 //Enable RX
	LinaRegs.SCIGCR1.bit.TXENA = 1;		 //Enable TX

	//Ensure IODFT is disabled
    LinaRegs.IODFTCTRL.bit.IODFTENA = 0x0;

    //Set transmission length
    LinaRegs.SCIFORMAT.bit.CHAR = 7;	 //Eight bits
    LinaRegs.SCIFORMAT.bit.LENGTH = 1;   //Two bytes

	//Set baudrate
    LinaRegs.BRSR.bit.SCI_LIN_PSL = 194;	 //9600 bps for SYSCLKOUT = 60 MHz
    LinaRegs.BRSR.bit.M = 5;

	//Enable interrupts
	LinaRegs.SCISETINT.bit.SETRXINT = 1; //Enable RX interrupt

    //Set interrupt priority
	LinaRegs.SCICLEARINTLVL.all = 0xFFFFFFFF; //Set Int level of all interrupts to LVL 0

    LinaRegs.SCIGCR1.bit.SWnRST = 1;  //bring out of software reset

	//Disable write to protected registers*/
	EDIS;
}



void error(void)
{
	errorcount++;
	//asm("     ESTOP0"); // Test failed!! Stop!
	//for (;;);
	RcvTaskPointerLin = &GetCmdByteLin;			// Initialize the CmdPacket Rcv Handler state machine ptr
	for (iLin=0; iLin<PktSizeLin; iLin++)
	{
		CmdPacketLin[iLin] = 0x0;
	}
	rdataALin = 0;
	sdataALin = 0;
}
