
#define	PktSizeLin				2

// Prototype statements for functions found within this file.
extern interrupt void Lina_Level0_ISR(void);

extern void PackTxBuffersLin(void);
extern void UnpackRxBuffersLin(void);
extern void SetupLin(void);
extern void error(void);


// Global variables
extern Uint16 sdataALin;     // Send data for SCI-A
extern Uint16 rdataALin;     // Received data for SCI-A


extern Uint32 LinL0IntVect;  //Vector for L0 interrupts

extern Uint16 iLin;			  //Generic iterator
extern int errorcount;

extern int16* Gui_wPanelVolt;
extern int16* Gui_wPanelCurrent;
extern int16* Gui_wBoostOutputVolt;
extern int16* Gui_wPanelOutputPower;
extern int16* Gui_wMPPT_Status;

extern int16 Gui_wLLCOutputVolt;//****************SAC


extern void (*RcvTaskPointerLin)(void); 			// State pointer for Command Packet Receive
extern Uint16	CmdPacketLin[PktSizeLin];


extern void GetCmdByteLin(void);
extern void EchoCmdByteLin(void);
extern void GetIndexByteLin(void);
extern void EchoIndexByteLin(void);
extern void GetDataByteLin(void);
extern void EchoDataByteLin(void);
extern void SendDataByteLin(void);
extern void CompleteByteLin(void);
