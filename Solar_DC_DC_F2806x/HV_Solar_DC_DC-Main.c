//----------------------------------------------------------------------------------
//  FILE:           Solar_DC_DC-Main.C
//
//  Description:    Solar MPPT DC-DC Stage
//                  The file drives duty on PWM1A and PWM2A for DC-DC boost stage on the HV Solar
//                  MPPT board. It also generates 50% duty with dead-band on PWM3A & PWM3B for
//                  the LLC stage on the same board.
//                  C28x ISR is triggered by the PWM 1 interrupt
//
//  Version:        1.0
//
//  Target:         TMS320F28069(PiccoloB),
//
//----------------------------------------------------------------------------------
//  Copyright Texas Instruments � 2004-2010
//----------------------------------------------------------------------------------
//  Revision History:
//----------------------------------------------------------------------------------
//  Date      | Description / Status
//----------------------------------------------------------------------------------
// March 22 , 2011  - File created (SC)
//----------------------------------------------------------------------------------
//
// PLEASE READ - Useful notes about this Project

// Although this project is made up of several files, the most important ones are:
//   "Solar_DC_DC-Main.C"   - this file
//      - Application Initialization, Peripheral config,
//      - Application management
//      - Slower background code loops and Task scheduling
//   "Solar_DC_DC-DevInit_F28xxx.C
//      - Device Initialization, e.g. Clock, PLL, WD, GPIO mapping
//      - Peripheral clock enables
//      - DevInit file will differ per each F28xxx device series, e.g. F280x, F2833x,
//   "Solar_DC_DC-DPL-ISR.asm
//      - Assembly level library Macros and any cycle critical functions are found here
//   "Solar_DC_DC-Settings.h"
//      - Global defines (settings) project selections are found here
//      - This file is referenced by both C and ASM files.
//
// Code is made up of sections, e.g. "FUNCTION PROTOTYPES", "VARIABLE DECLARATIONS" ,..etc
//  each section has FRAMEWORK and USER areas.
//  FRAMEWORK areas provide useful ready made "infrastructure" code which for the most part
//  does not need modification, e.g. Task scheduling, ISR call, GUI interface support,...etc
//  USER areas have functional example code which can be modified by USER to fit their appl.
//
// Code can be compiled with various build options (Incremental Builds IBx), these
//  options are selected in file "Solar_DC_DC-Settings.h".  Note: "Rebuild All" compile
//  tool bar button must be used if this file is modified.
//----------------------------------------------------------------------------------
#include "PeripheralHeaderIncludes.h"
#include "F2806x_EPWM_defines.h"
#include "Solar_DC_DC-Settings.h"
#include "IQmathLib.h"

#include "SineAnalyzer.h"

#include "DPlib.h"
#include "mppt_incc.h"
#include "mppt_pno.h"

#include <math.h>
#include <stdlib.h>


//#define FLASH

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// FUNCTION PROTOTYPES
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// Add protoypes of functions being used in the project here 
void DeviceInit(void);
void InitFlash();
void MemCopy();
//-------------------------------- DPLIB --------------------------------------------
void InitECana(void);
void InitCAN_MB(void);
void PWM_1ch_UpDwnCnt_CNF(int16 n, int16 period, int16 mode, int16 phase);
void PWM_2ch_UpDwnCnt_CNF(int16 n, int16 period, int16 phase);// Added 2-ch mode for PWM A and B outputs
void PWM_ComplPairDB_CNF(int16 n, int16 period, int16 mode, int16 phase);
void PWM_ComplPairDB_UpdateDB(int16 n, int16 dbRED, int16 dbFED);
void ADC_SOC_CNF(int ChSel[], int TrigSel[], int ACQPS[], int IntChSel, int mode);

interrupt void SECONDARY_ISR(void);
// -------------------------------- FRAMEWORK --------------------------------------
// State Machine function prototypes
//----------------------------------------------------------------------------------
// Alpha states
void A0(void);  //state A0
void B0(void);  //state B0
void C0(void);  //state C0

// A branch states
void A1(void);  //state A1
void A2(void);  //state A2
void A3(void);  //state A3
void A4(void);  //state A4

// B branch states
void B1(void);  //state B1
void B2(void);  //state B2

// C branch states
void C1(void);  //state C1
void C2(void);  //state C2
void C3(void);  //state C3
void C4(void);  //state C4

// Variable declarations
void (*Alpha_State_Ptr)(void);  // Base States pointer
void (*A_Task_Ptr)(void);       // State pointer A branch
void (*B_Task_Ptr)(void);       // State pointer B branch
void (*C_Task_Ptr)(void);       // State pointer C branch

//Initialization functions
void Configure_PWM(void);
void Configure_ADC(void);

//LED Control & defines
#define LED_W_BR    0
#define LED_R_BR    1
#define LED_G_BR    2
#define LED_B_BR    3
#define LED_ORG_BR  4
#define LED_CYA_BR  5
#define LED_MAG_BR  6
#define LED_YEL_BR  7
#define LED_R_FL    8

//CAN parameter defines
#define CAN_MAX_UNACK   10
#define HTSK_OHVAL      32768

Uint16 ledSetMode;      //Variable for setting LED Display mode
Uint16 fade;            //Fade counter for breathing LED
Uint16 fade_dir;        //Direction for LED fading, 0 up, 1 down

Uint16 CAN_noACK;       //Counter for number of non-acknowledged CAN messages

Uint16 HTSK_TP;         //Heatsink temperature probe
Uint16 EXTHTSK_TP;         //Heatsink temperature probe

void RGBWSetColor(Uint16 redDuty, Uint16 greenDuty, Uint16 blueDuty, Uint16 whiteDuty);
void RGBModeControl(Uint16 DisplayMode);

//----------------------------------------------------------------------------------

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// VARIABLE DECLARATIONS - GENERAL
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// -------------------------------- FRAMEWORK --------------------------------------
int16 VTimer0[4];   // Virtual Timers slaved off CPU Timer 0
int16 VTimer1[4];   // Virtual Timers slaved off CPU Timer 1
int16 VTimer2[4];   // Virtual Timers slaved off CPU Timer 2

// Used for running BackGround in flash, and ISR in RAM
extern Uint16 *RamfuncsLoadStart, *RamfuncsLoadEnd, *RamfuncsRunStart;

// Used for ADC Configuration 
int ChSel[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int TrigSel[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int ACQPS[16] = {6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6};

//---------------------------------------------------------------------------
// Used to indirectly access all EPWM modules
volatile struct EPWM_REGS *ePWM[] = 
                  { &EPwm1Regs,         //intentional: (ePWM[0] not used)
                    &EPwm1Regs,
                    &EPwm2Regs,
                    &EPwm3Regs,
                    &EPwm4Regs,
                    &EPwm5Regs,
                    &EPwm6Regs,
                    &EPwm7Regs,
                  };

// Used to indirectly access all Comparator modules
volatile struct COMP_REGS *Comp[] = 
                  { &Comp1Regs,         //intentional: (Comp[0] not used)
                    &Comp1Regs,
                    &Comp2Regs,
                    &Comp3Regs
                  };
// ---------------------------------- USER -----------------------------------------

//long Pgain;               // PID P gain
//long Igain;               // PID I gain
//long Dgain;               // PID D gain
long Dmax_I;


// ---------------------------- DPLIB Net Pointers ---------------------------------
// Declare net pointers that are used to connect the DP Lib Macros  here 

// ADC
extern volatile long *ADCDRV_1ch_Rlt1;  // instance #1, IL
extern volatile long *ADCDRV_1ch_Rlt2;  // instance #2, Vb, Boost output volt
extern volatile long *ADCDRV_1ch_Rlt3;  // instance #3, Vp, Panel output volt
extern volatile long *ADCDRV_1ch_Rlt4;  // instance #4, Ics, LLC stage primary current

// CONTROL_2P2Z
extern volatile long *CNTL_2P2Z_Ref1;   // instance #1
extern volatile long *CNTL_2P2Z_Out1;   // instance #1
extern volatile long *CNTL_2P2Z_Fdbk1;  // instance #1
extern volatile long *CNTL_2P2Z_Coef1;  // instance #1
extern volatile long CNTL_2P2Z_DBUFF1[5];

// CONTROL_2P2Z
extern volatile long *CNTL_2P2Z_Ref2;   // instance #2
extern volatile long *CNTL_2P2Z_Out2;   // instance #2
extern volatile long *CNTL_2P2Z_Fdbk2;  // instance #2
extern volatile long *CNTL_2P2Z_Coef2;  // instance #2
extern volatile long CNTL_2P2Z_DBUFF2[5];

// Boost-ch1
extern volatile long *PWMDRV_1ch_UpDwnCnt_Duty1;    // instance #1, EPWM1
// Boost-ch2
extern volatile long *PWMDRV_1ch_UpDwnCnt_Duty2;    // instance #2, EPWM2
// Instrumentation-ch4
extern volatile long *PWMDRV_1ch_UpDwnCnt_Duty4;    // instance #3, EPWM4



// Instrumentation-ch4
extern volatile long *PWMDRV_1ch_UpDwnCnt_Duty6;    // instance #4, EPWM6




extern volatile long *PWMDRV_LLC_ComplPairDB_Duty3;
extern volatile long *PWMDRV_LLC_ComplPairDB_Period3;
extern volatile long *PWMDRV_LLC_ComplPairDB_DeadBandRED3;
extern volatile long *PWMDRV_LLC_ComplPairDB_DeadBandFED3;

//MATH_AVG - instance #1
extern volatile long *MATH_EMAVG_In1;
extern volatile long *MATH_EMAVG_Out1;
extern volatile long MATH_EMAVG_Multiplier1;

//MATH_AVG - instance #2
extern volatile long *MATH_EMAVG_In2;
extern volatile long *MATH_EMAVG_Out2;
extern volatile long MATH_EMAVG_Multiplier2;


// ---------------------------- DPLIB Variables ---------------------------------
// Declare the net variables being used by the DP Lib Macro here 
volatile long IL_raw, IL,Ics; 
volatile long IL_avg; 
volatile long Vb_fb,Vb_fb_Avg; //Boost output volt
volatile long Vp_fb,Vp_fb_Avg; //Panel output volt

volatile long DutyA, DutyLLC; 
volatile long Vp_fb, Boost_IL_cmd;
volatile long Duty4A;
volatile long VpvRef,VpvRef_MPPT; // Set point for the Panel volt
volatile long PanelPower, PanelPower_Prev, Powerdiff;


long    temp_zero;

int16   periodLLC, dbred, dbfed, db_dly_cntr, soft_start;


#pragma DATA_SECTION(CNTL_2P2Z_CoefStruct1, "CNTL_2P2Z_Coef"); 
#pragma DATA_SECTION(CNTL_2P2Z_CoefStruct2, "CNTL_2P2Z_Coef"); 
struct CNTL_2P2Z_CoefStruct CNTL_2P2Z_CoefStruct1;
struct CNTL_2P2Z_CoefStruct CNTL_2P2Z_CoefStruct2;

long Pgain_I,Igain_I,Dgain_I,Dmax_I;
long Pgain_V,Igain_V,Dgain_V,Dmax_V;

// MPPT tracking , declare both PnO and Incc objects so that comparisons can be done
// Instance for mppt incremental conductance loop
mppt_incc mppt_incc1 = mppt_incc_DEFAULTS;
mppt_pno  mppt_pno1 = mppt_pno_DEFAULTS;

int16 MPPT_slew, inverter_connected, inverter_mode;
int16 Enable_mppt, GPIO_status, Operation_mode, LLC_PWM_ON, GPIO_cnt, Start_DC_DC;

int16   Run_MPPT;
int16   mppt_firstTime;
int16   perturbation_direction;
int16   Gui_Powerdiff;
int16   Gui_PanelPower_Prev;

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// VARIABLE DECLARATIONS - CCS WatchWindow / GUI support
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

// -------------------------------- FRAMEWORK --------------------------------------

//GUI support variables
// sets a limit on the amount of external GUI controls - increase as necessary
int16 LedBlinkCnt;

// ---------------------------------- USER -----------------------------------------

// Monitor ("Get")  // Display as:
int16 Gui_Vb_fb;        // Q06. Variable used for displaying Vb on DC-DC GUI
int16 Gui_PanelPower; 
int16 Gui_IL_avg;       //Variable used for displaying average IL (panel current) on DC-DC GUI
int16 Gui_Vp_fb;        //Variable used for displaying Vp (panel volt) on DC-DC GUI

// History arrays are used for Running Average calculation (boxcar filter)
// HistorySize is defined in Solar_DC_DC_Settings.h file
// Used for CCS display and GUI only, not part of control loop processing
Uint16 Hist_IL[HistorySize];
Uint16 Hist_Vb_fb[HistorySize];
Uint16 Hist_Vp_fb[HistorySize];
//Uint16 Hist_Ics[HistorySize];
//Uint16 Hist_VrectRMS[HistorySize];
//Uint16 Hist_Freq_Vin[HistorySize];
//Uint16 Hist_KDCM[HistorySize];

//Scaling Constants (exact value calibrated per board)associated with variables used for DC-DC side GUI
Uint16 K_IL_avg;    //
Uint16 K_Vb_fb; //
Uint16 K_Vp_fb; //

//Scaling Constants associated with variables used for Inverter side GUI
Uint16 Kinv_IL_avg; //
Uint16 Kinv_Vb_fb;  //
Uint16 Kinv_Vp_fb;  //


// Variables for background support only (no need to access)
int16 i;                        // common use incrementer
Uint32 HistPtr, temp_Scratch;   // Temp here means Temporary


//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
// MAIN CODE - starts here
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
void main(void)
{
//=================================================================================
//  INITIALIZATION - General
//=================================================================================

    // The DeviceInit() configures the clocks and pin mux registers
    // The function is declared in Solar_DC_DC-DevInit_F2803/2x.c,
    // Please ensure/edit that all the desired components pin muxes
    // are configured properly that clocks for the peripherals used
    // are enabled, for example the individual PWM clock must be enabled
    // along with the Time Base Clock


//-------------------------------- FRAMEWORK --------------------------------------

// Only used if running from FLASH
// Note that the variable FLASH is defined by the compiler with -d FLASH

//MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);

//#ifdef FLASH
// Copy time critical code and Flash setup code to RAM
// The  RamfuncsLoadStart, RamfuncsLoadEnd, and RamfuncsRunStart
// symbols are created by the linker. Refer to the linker files. 
    MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);
    //memcpy(&secureRamFuncs_runstart, &secureRamFuncs_loadstart, (Uint32)&secureRamFuncs_loadsize);
// Call Flash Initialization to setup flash waitstates
// This function must reside in RAM
    InitFlash();    // Call the flash wrapper init function
//#endif //(FLASH)

    DeviceInit();   // Device Life support & GPIO
// Timing sync for background loops
// Timer period definitions found in PeripheralHeaderIncludes.h
    CpuTimer0Regs.PRD.all =  mSec1;     // A tasks
    CpuTimer1Regs.PRD.all =  mSec500;    // B tasks
    CpuTimer2Regs.PRD.all =  mSec5;    // C tasks

// Tasks State-machine init
    Alpha_State_Ptr = &A0;
    A_Task_Ptr = &A1;
    B_Task_Ptr = &B1;
    C_Task_Ptr = &C1;

    VTimer0[0] = 0;
    VTimer1[0] = 0;
    VTimer2[0] = 0;
    LedBlinkCnt = 5;

// ---------------------------------- USER -----------------------------------------
//  put common initialization/variable definitions here

    K_IL_avg=19455;
    Kinv_IL_avg=(19455.0*100.0)/2048.0;
    K_Vb_fb=32959;//K_Vb_fb=(515/512)*32767 = 32959;
    Kinv_Vb_fb=(32959.0*10.0)/64.0;

    K_Vp_fb=32767;  // K_Vp_fb=(512/512)*32767 = 32767; 15-bit GUI variable Gui_Vp_fb is denoted in Q6
    Kinv_Vp_fb=(32767.0*10.0)/64.0;

    dbred=25;//Initialize dead band for LLC PWM
    dbfed=25;
    db_dly_cntr=0;

//==================================================================================
//  INCREMENTAL BUILD OPTIONS - NOTE: selected via {Solar_DC_DC-Settings.h
//==================================================================================
// ---------------------------------- USER -----------------------------------------


    Configure_PWM();

    Configure_ADC();

    fade_dir = 0;
    fade = 0;
    ledSetMode = LED_W_BR;

    CAN_noACK = CAN_MAX_UNACK; //Initially set CAN to not online

    HTSK_TP = 0;
    EXTHTSK_TP = 0;

    DPL_Init();
//--------------------------------------------------------------------------------
//----------------------------------------------------------------------
#if (INCR_BUILD == 3)   // Closed Current Loop & closed input volt loop dc-dc boost(Panel Volt Output,
                        // or, DC-DC Boost Volt Input)
//----------------------------------------------------------------------
// Lib Module connection to "nets" 
    //----------------------------------------
    // Connect the PWM Driver input to an input variable, Open Loop System
    ADCDRV_1ch_Rlt1 = &IL_raw;
    ADCDRV_1ch_Rlt2 = &Vb_fb;
    ADCDRV_1ch_Rlt3 = &Vp_fb;
    ADCDRV_1ch_Rlt4 = &Ics;

    // Math_avg block connections - Instance 1
    MATH_EMAVG_In1=&IL_raw;
    //MATH_EMAVG_In1=&IL;
    MATH_EMAVG_Out1=&IL_avg;
    MATH_EMAVG_Multiplier1=_IQ30(0.008);//(0.030);

    // Math_avg block connections - Instance 2
    MATH_EMAVG_In2=&Vp_fb;//&Vb_fb;
    MATH_EMAVG_Out2=&Vp_fb_Avg;//&Vb_fb_Avg;
    MATH_EMAVG_Multiplier2=_IQ30(0.008);//(0.00025);

        //connect the 2P2Z connections, for the inner Current Loop, Loop1
    CNTL_2P2Z_Ref1 = &Boost_IL_cmd;
    CNTL_2P2Z_Out1 = &DutyA;
    CNTL_2P2Z_Fdbk1= &IL_raw;
    //CNTL_2P2Z_Fdbk1= &IL;
    CNTL_2P2Z_Coef1 = &CNTL_2P2Z_CoefStruct1.b2;


    //Connect the 2P2Z connections, for the voltage loop for Vin (panel voltage) control, Loop2.
    //*******This volt loop has +ve feedback (compared to the conventional -ve feedback) since for higher Vin, i.e., panel volt
    //we want higher control output, i.e., in this case higher boost current and vice versa. Thus the net
    //connections below are reversed for feedback and reference terminals**************
    CNTL_2P2Z_Fdbk2 = &VpvRef;//PV panel reference(VpvRef) is connected to the controller feedback net (CNTL_2P2Z_Fdbk2)
    CNTL_2P2Z_Out2 = &Boost_IL_cmd;
    CNTL_2P2Z_Ref2= &Vp_fb;//PV panel feedback(Vp_fb) is connected to the controller reference net (CNTL_2P2Z_Ref2)
    CNTL_2P2Z_Coef2 = &CNTL_2P2Z_CoefStruct2.b2;

    PWMDRV_1ch_UpDwnCnt_Duty1 = &DutyA;
    PWMDRV_1ch_UpDwnCnt_Duty2 = &DutyA;


    // Initialize the net variables
    DutyA =_IQ24(0.0);
    Duty4A =_IQ24(0.0);
    IL_avg = _IQ24(0.0);
    IL = _IQ24(0.0);
    IL_raw = _IQ24(0.0);
    Vb_fb_Avg = _IQ24(0.0);
    Boost_IL_cmd = _IQ24(0.0);
    Vp_fb = _IQ24(0.9);
    Vp_fb_Avg = _IQ24(0.9);
    VpvRef = _IQ24(0.9);
    temp_zero = 0;
    Operation_mode = 0;//DC-DC stage status flag. 
                 //Operation_mode = 0 =>Boost OFF & LLC ON, Operation_mode = 1 =>Boost ON & LLC ON
                 //Operation_mode = 2 =>Boost OFF & LLC OFF,
    
    LLC_PWM_ON = 0;//LLC initially off. Set this flag to 1 after soft-start.
    //LLC_PWM_ON = 1;//Used as LLC PWM status flag. Initialized to 1 since LLC PWM is initially ON
    
    soft_start = 0;
    
    GPIO_cnt = 0; //Used as a delay counter before starting MPPT
    
    Start_DC_DC = 0;
    




    //////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////
    inverter_connected = 0; //Default setting is 1, implying that inverter is connected.
    //////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////



    inverter_mode = 0;//Used for showing EVM status (whether the inverter is connected or not)
    
    //--------------------------------------------------------------------------------
    // Coefficients for Inner Current Loop
    //--------------------------------------------------------------------------------
    // Coefficients for Inner Current Loop
    // PID coefficients & Clamp values - Current loop (Q26), 100kHz Cloop sampling
    


    /////////////////////////////////////////////////////////////////////////////
    Dmax_I  = _IQ24(0.75); //Min Vin=200V, Max Vout=400V, Therefore Boost PWM max duty = 0.5
    /////////////////////////////////////////////////////////////////////////////


    CNTL_2P2Z_CoefStruct1.b2   =_IQ26(0.06721);// B2
    CNTL_2P2Z_CoefStruct1.b1   =_IQ26(-0.208);// B1
    CNTL_2P2Z_CoefStruct1.b0   =_IQ26(0.1432);  // B0

    CNTL_2P2Z_CoefStruct1.a2   =_IQ26(-0.028);                         // A2 = 0
    CNTL_2P2Z_CoefStruct1.a1   =_IQ26(1.028);                           // A1 = 1
    CNTL_2P2Z_CoefStruct1.max  = Dmax_I;                                //Clamp Hi
    CNTL_2P2Z_CoefStruct1.min  =_IQ24(0.0);                         //Clamp Min


//------------------------------------------------------------------------------
// Coefficients for Voltage Loop - Regulates Boost Input Volt dictated by MPPT algo
//------------------------------------------------------------------------------
    // PID coefficients & Clamp values - Voltage loop (Q26), 50kHz Vloop sampling




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    Dmax_V  = _IQ24(0.765);//Max output from Vloop controller sets the Max boost current. At 200Vin, 550W input, Iboost=2.625A
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //Max boost current is 9.1A (set by the CS amplifier gain of 39.2 and CS sense resistor 0.01 ohm). Therefore,
    //per unit boost current max value is limited to 2.625/9.1 = 0.2885. Add 10% margin => Max current = 0.31 pu




    Pgain_V = _IQ26(0.25);//
    Igain_V = _IQ26(0.0005);//(0.015);//0.0002//(0.0001);
    Dgain_V =_IQ26(0.0);  //


    // Coefficient init --- Coeeficient values in Q26
    // Use IQ Maths to generate floating point values for the CLA
    CNTL_2P2Z_CoefStruct2.b2   =Dgain_V;                            // B2
    CNTL_2P2Z_CoefStruct2.b1   =(Igain_V-Pgain_V-Dgain_V-Dgain_V);  // B1
    CNTL_2P2Z_CoefStruct2.b0   =(Pgain_V + Igain_V + Dgain_V);      // B0
    CNTL_2P2Z_CoefStruct2.a2   =0.0;                                // A2 = 0
    CNTL_2P2Z_CoefStruct2.a1   =_IQ26(1.0);                         // A1 = 1
    CNTL_2P2Z_CoefStruct2.max  =Dmax_V;                             //Clamp Hi
    CNTL_2P2Z_CoefStruct2.min  =_IQ24(0.0);                         //Clamp Min

    // MPPT testing related code
    // mppt incc
    mppt_incc1.IpvH = _IQ(0.0001);
    mppt_incc1.IpvL = _IQ(-0.0001);
    mppt_incc1.VpvH = _IQ(0.0001);
    mppt_incc1.VpvL = _IQ(-0.0001);
    mppt_incc1.MaxVolt = _IQ(0.9);
    mppt_incc1.MinVolt = _IQ(0.0);
    mppt_incc1.Stepsize = _IQ(0.001);
    mppt_incc1.mppt_first=1;
    mppt_incc1.mppt_enable=0;
    
    //mppt pno
    mppt_pno1.DeltaPmin = _IQ(0.00001);
    mppt_pno1.MaxVolt = _IQ(0.9);
    mppt_pno1.MinVolt = _IQ(0.0);
    mppt_pno1.Stepsize = _IQ(0.001);

#endif // (INCR_BUILD == 3),
//----------------------------------------------------------------------
//--------------------------------------------------------------------------------


    mppt_firstTime=1;
    Run_MPPT=0;
    Gui_Powerdiff=_IQ6(0.0);
    Gui_PanelPower_Prev=_IQ6(0.0);
    Powerdiff=_IQ24(0.0);
    PanelPower_Prev=_IQ24(0.0);
    VpvRef_MPPT=_IQ24(0.0);
    perturbation_direction=0;
    MPPT_slew=0;
    Enable_mppt=0;
    GPIO_status=1;//Initialize this to 1 to keep MPPT diabled on power up until we get a LOW GPIO
                    // signal(GPIO16) from the inverter board


//--------------------------------------------------------------------------------


        EALLOW;
//Activate /TZ1 under s/w control to shut off PWM1A, PWM2A under DC bus overvoltage condition
        //EPwm1Regs.TZSEL.bit.OSHT1 = 1;
        //EPwm2Regs.TZSEL.bit.OSHT1 = 1;
        EPwm3Regs.TZSEL.bit.OSHT1 = 1;

//Use Comparator 3 (associated with ADC-A6) to implement Cycle by cycle trip on Vbus (boost stage)overvoltage
        Comp3Regs.COMPCTL.bit.COMPDACEN  =0x1;
        Comp3Regs.COMPCTL.bit.SYNCSEL    =0x0;  // asynchronous version of the COMP signal is passed to the EPWM/GPIO module
        Comp3Regs.COMPCTL.bit.CMPINV     =0x0;  // Output of the comparator is passed directly
        Comp3Regs.COMPCTL.bit.COMPSOURCE =0x0;  // inverting input of the comparator is connected to the internal DAC
        Comp3Regs.DACVAL.bit.DACVAL      =808; //Set DAC input to peak Vout (Boost stage) OVP Threshold
                                                //400; //1023*(200V/511V)=400<=>200V @Boost Vout,
                                                // 652; //1023*(326V/511V)=652<=>326V @Boost Vout
                                                // 701; //1023*(350V/511V)=701<=>350V @Boost Vout
                                                // 760; //1023*(380V/511V)=760<=>380V @Boost Vout
                                                // 808; //1023*(404V/511V)=808<=>404V @Boost Vout
                                                // 840; //1023*(420V/511V)=840<=>420V @Boost Vout

        AdcRegs.COMPHYSTCTL.bit.COMP3_HYST_DISABLE = 0x1;

// Cycle by cycle trip for overvoltage protection
        EPwm1Regs.DCTRIPSEL.bit.DCAHCOMPSEL=DC_COMP3OUT;
        EPwm1Regs.TZDCSEL.bit.DCAEVT2=TZ_DCAH_HI;
        EPwm1Regs.DCACTL.bit.EVT2SRCSEL = DC_EVT2;
        EPwm1Regs.DCACTL.bit.EVT2FRCSYNCSEL=DC_EVT_ASYNC;
        EPwm1Regs.TZSEL.bit.DCAEVT2=0x1;

        EPwm2Regs.DCTRIPSEL.bit.DCAHCOMPSEL=DC_COMP3OUT;
        EPwm2Regs.TZDCSEL.bit.DCAEVT2=TZ_DCAH_HI;
        EPwm2Regs.DCACTL.bit.EVT2SRCSEL = DC_EVT2;
        EPwm2Regs.DCACTL.bit.EVT2FRCSYNCSEL=DC_EVT_ASYNC;
        EPwm2Regs.TZSEL.bit.DCAEVT2=0x1;


        // Cycle by cycle interrupt for CPU halt trip
        EPwm3Regs.TZSEL.bit.CBC6=0x1;
        EPwm1Regs.TZSEL.bit.CBC6=0x1;
        EPwm2Regs.TZSEL.bit.CBC6=0x1;

// What do we want the OST/CBC events to do?
// TZA events can force EPWMxA
// TZB events can force EPWMxB
        EPwm3Regs.TZCTL.bit.TZA = TZ_FORCE_LO; // EPWM3A will go low
        EPwm3Regs.TZCTL.bit.TZB = TZ_FORCE_LO; // EPWM3B will go low

        EPwm1Regs.TZCTL.bit.TZA = TZ_FORCE_LO; // EPWM1A will go low
        EPwm1Regs.TZCTL.bit.TZB = TZ_FORCE_LO; // EPWM1B will go low

        EPwm2Regs.TZCTL.bit.TZA = TZ_FORCE_LO; // EPWM2A will go low
        EPwm2Regs.TZCTL.bit.TZB = TZ_FORCE_LO; // EPWM2B will go low

        EDIS;

//=================================================================================
//  CAN INITIALIZATION
//=================================================================================

    InitCAN_MB();


//====================================================================================
// INTERRUPTS & ISR INITIALIZATION (best to run this section after other initialization)
//====================================================================================

//Also Set the appropriate # define's in the Solar_DC_DC-Settings.h 
//to enable interrupt management in the ISR
    EALLOW;
// Set up C28x Interrupt

// ADC EOC based ISR trigger
//  PieVectTable.ADCINT1 = &DPL_ISR;            // Map Interrupt
//  PieCtrlRegs.PIEIER1.bit.INTx1 = 1;          // PIE level enable, Grp1 / Int1, ADCINT1
//  AdcRegs.INTSEL1N2.bit.INT1SEL = 4;          // ADC Channel 4 EOC causes ADCInterrupt 1
//  IER |= M_INT1;                              // Enable CPU INT1 group:

// PWM based ISR trigger
    PieVectTable.EPWM1_INT = &DPL_ISR;          // Map Interrupt
    //PieVectTable.LIN0INTA  = &Lina_Level0_ISR;
    PieCtrlRegs.PIEIER3.bit.INTx1 = 1;          // PIE level enable, Grp3 / Int1, ePWM1
    //LIN Interrupt
    PieCtrlRegs.PIEIER9.bit.INTx3=1;     // PIE Group 9, INT3

    EPwm1Regs.CMPB = 80;                        // ISR trigger point
    EPwm1Regs.ETSEL.bit.INTSEL = ET_CTRU_CMPB;  // INT on CompareB-Up event
    EPwm1Regs.ETSEL.bit.INTEN = 1;              // Enable INT
 //   EPwm1Regs.ETPS.bit.INTPRD = ET_1ST;         // Generate INT on every event

    EPwm1Regs.ETPS.bit.INTPRD = ET_2ND;         // Generate INT on every event
    // Enable Peripheral, global Ints and higher priority real-time debug events:
    IER = 0x100;    // Enable CPU INT
    IER |= M_INT1;
    IER |= M_INT3;                              // Enable CPU INT3 connected to EPWM1-6 INTs:
     
    EINT;                                       // Enable Global interrupt INTM
    ERTM;                                       // Enable Global realtime interrupt DBGM
    EDIS;



    //Wait for SCI to be idle and ready for transmission
    //while(LinaRegs.SCIFLR.bit.IDLE == 1);
//=================================================================================
//  BACKGROUND (BG) LOOP
//=================================================================================

//--------------------------------- FRAMEWORK -------------------------------------
    for(;;)  //infinite loop
    {

        /////////////////
        // State machine entry & exit point
        //===========================================================
        (*Alpha_State_Ptr)();   // jump to an Alpha state (A0,B0,...)
        //===========================================================

    }
} //END MAIN CODE
//============================================================================================

//=============================================================================================
// CONFIG : Function Call to configure PWM Module on startup
//=============================================================================================
void Configure_PWM(void){

#define period 600  //600 cycles -> 100KHz @60MHz CPU, PWM period for 2Ph Interleaved Boost stage
#define phase 300   //Phase shift for slave PWM in 2Ph IL Boost stage
#define period_LLC 500 //545=>110kHz,,460=>130kHz, 500 cycles -> LLC freq 120KHz @60MHz CPU

#define period_LLC_HI 448 //448 =>134kHz, -> LLC freq 134KHz @60MHz CPU

#define period_instr_pwm 120    //120 => 500k @60MHz CPU, period for PWM channels used for instrumentation

#define period_LED 30000    //2KHz @60Mhz CPU, PWM period for LED dimming

    // Configure PWM1 for 100Khz switching Frequency
    PWM_1ch_UpDwnCnt_CNF(1, period, 1, 0);
    // Configure PWM2 for 100Khz switching Frequency
    PWM_1ch_UpDwnCnt_CNF(2, period, 0, 300);

    //Configure PWM4 for 1KHz LED dimming
    PWM_2ch_UpDwnCnt_CNF(4, period_LED, 0);
    //Configure PWM6 for 1KHz LED dimming
    PWM_2ch_UpDwnCnt_CNF(6, period_LED, 0);

    (*ePWM[4]).CMPA.half.CMPA = 0;//period_LED/32;
    (*ePWM[4]).CMPB = 0;

    (*ePWM[6]).CMPA.half.CMPA = 0;//period_LED/32;
    (*ePWM[6]).CMPB = 0;


    PWM_ComplPairDB_CNF(3, period_LLC_HI, 1, 0);
    (*ePWM[3]).CMPA.half.CMPA = period_LLC_HI/2;

    if((INCR_BUILD == 1)||(INCR_BUILD == 2))
    {
        PWM_ComplPairDB_CNF(3, period_LLC, 1, 0);
        (*ePWM[3]).CMPA.half.CMPA = period_LLC/2;
        PWM_ComplPairDB_UpdateDB(3,dbred,dbred);
    }

    if(INCR_BUILD == 3)
    {
        dbred = (period_LLC_HI/2)-50;
        PWM_ComplPairDB_UpdateDB(3,dbred,dbred);//For Build 3, set low initial value of LLC duty using higher DB
    }

    EALLOW;
    SysCtrlRegs.PCLKCR1.bit.EPWM7ENCLK = 1;  // ePWM7
// Setup TBCLK
    EPwm7Regs.TBPRD = 3000;           // Set timer period 3000 TBCLKs
    EPwm7Regs.TBPHS.half.TBPHS = 0x0000;           // Phase is 0
    EPwm7Regs.TBCTR = 0x0000;                      // Clear counter
// Setup counter mode
    EPwm7Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up
    EPwm7Regs.TBCTL.bit.PHSEN = TB_DISABLE;        // Disable phase loading
    EPwm7Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;       // Clock ratio to SYSCLKOUT
    EPwm7Regs.TBCTL.bit.CLKDIV = TB_DIV1;
    EPwm7Regs.TBCTL.bit.SYNCOSEL = TB_CTR_ZERO;
// Configure the Start of Conversion for the ADC.
    EPwm7Regs.ETSEL.bit.INTEN = 1;               //
    EPwm7Regs.ETSEL.bit.INTSEL = ET_CTR_PRD;   // Select Int from counter = PRD
    EPwm7Regs.ETPS.bit.INTPRD = ET_1ST;        // Generate pulse on 1st event

    PieVectTable.EPWM7_INT  = &SECONDARY_ISR;
    IER   |= M_INT3;                                // Enable CPU INT3 for capture interrupt
    PieCtrlRegs.PIEIER3.bit.INTx7       = 1;  // Enable EPWM7 int in PIE group 3
    EDIS;

}
//=========================================================================================================
//End of Main PWM Config
//==========================================================================================================


//=============================================================================================
// CONFIG : Function Call to configure ADC Module on startup
//=============================================================================================
void Configure_ADC(void){

#define     ILR         AdcResult.ADCRESULT1        //Q12
#define     Vb_fbR      AdcResult.ADCRESULT2        //Q12
#define     Vp_fbR      AdcResult.ADCRESULT3        //Q12
#define     IcsR        AdcResult.ADCRESULT4        //Q12
#define     Thrm_Htsk   AdcResult.ADCRESULT5        //Q12

            // ADC Channel Selection for C2000EVM
    ChSel[0] = 2;       // Dummy read for first
    ChSel[1] = 2;       // A2 - ILb_fb, Boost inductor current
    ChSel[2] = 1;       // A1 - Vb_fb, Boost output volt
    ChSel[3] = 0;       // A0 - Vp_fb, Panel voltage
    ChSel[4] = 4;       // A4 - Ics_fb, LLC stage primary current
    ChSel[5] = 7;       // A7 - Thermocouple

    // ADC Trigger Selection
    TrigSel[0] = ADCTRIG_EPWM1_SOCA;    // ePWM1, ADCSOCA
    TrigSel[1] = ADCTRIG_EPWM1_SOCA;    // ePWM1, ADCSOCA
    TrigSel[2] = ADCTRIG_EPWM1_SOCA;    // ePWM1, ADCSOCA
    TrigSel[3] = ADCTRIG_EPWM1_SOCA;    // ePWM1, ADCSOCA
    TrigSel[4] = ADCTRIG_EPWM1_SOCA;    // ePWM1, ADCSOCA
    TrigSel[5] = ADCTRIG_EPWM1_SOCA;    // ePWM1, ADCSOCA

    // Configure ADC
    ADC_SOC_CNF(ChSel, TrigSel, ACQPS, 17, 0);

    // Configure ePWMs to generate ADC SOC pulses
    EPwm1Regs.ETSEL.bit.SOCAEN = 1;                 // Enable ePWM1 SOCA pulse
    EPwm1Regs.ETSEL.bit.SOCASEL = ET_CTR_ZERO;      // SOCA from ePWM1 Zero event
    EPwm1Regs.ETPS.bit.SOCAPRD = ET_1ST;            // Trigger ePWM1 SOCA on every event
}
//=============================================================================================
// CONFIG : Function Call to configure PWM Module on startup
//=============================================================================================


//=============================================================================================
// CONFIG : Function Call to configure CAN Mailboxes on startup
//=============================================================================================
void InitCAN_MB(void){
    // CAN MSGID Formatting:
    // Bit 31: Contains the CAN address size 0 : 11 bit, 1 : 29 bit
    // Bits 30, 29: Acknowledgment and request config - set both to 0
    // Bit  28:0: CAN address field, bits 28:18 are used for the address when bit 31 is 0 (11-bit address)
    // For 11-bit CAN, use a left bitwise shift of 18

    struct ECAN_REGS ECanaShadow;

    InitECana();    // CAN Bus Configuration

    //Bitshift CAN ID (In Hex) left by 18
    //ECanaMboxes.MBOX0.MSGID.all = 0x04000000;          // Address 0x100
    //ECanaMboxes.MBOX1.MSGID.all = 0x0A000000;          // Address 0x280
    ECanaMboxes.MBOX0.MSGID.all = 0x1A400000;          // Address 0x690
    ECanaMboxes.MBOX1.MSGID.all = 0x1A440000;          // Address 0x691
    ECanaMboxes.MBOX2.MSGID.all = 0x1A480000;          // Address 0x692 - control from system

    // Configure Mailbox under test as a Transmit mailbox
    ECanaShadow.CANMD.all = ECanaRegs.CANMD.all;
    ECanaShadow.CANMD.bit.MD0 = 0;
    ECanaShadow.CANMD.bit.MD1 = 0;
    ECanaShadow.CANMD.bit.MD2 = 1;
    ECanaRegs.CANMD.all = ECanaShadow.CANMD.all;


    // Enable Mailbox
    ECanaShadow.CANME.all = ECanaRegs.CANME.all;
    ECanaShadow.CANME.bit.ME0 = 1;
    ECanaShadow.CANME.bit.ME1 = 1;
    ECanaShadow.CANME.bit.ME2 = 1;
    ECanaRegs.CANME.all = ECanaShadow.CANME.all;


    // Set how many bytes to be transmitted
    ECanaMboxes.MBOX0.MSGCTRL.bit.DLC = 8;
    ECanaMboxes.MBOX1.MSGCTRL.bit.DLC = 8;


}
//=============================================================================================
// CONFIG : Function Call to configure CAN Mailboxes on startup
//=============================================================================================


//=============================================================================================
// ISR : 20kHz Interrupt Service Routine for time critical functions
//=============================================================================================
interrupt void SECONDARY_ISR(void)
{
    EINT;
//-----------------------------------------------------------------------------------------------
//Moved MPPT from 50mS background loop to 20kHz ISR with MPPT_Slew variable increased from 1 to 500

//Following is an implementation of perturb and observe algorithm to track the maximum power point.
//The algorithm is based on re-inforcement learning and continually perturbs the operating point
//observing the change in power and using a reward and punishment based algorithm to get to the maximum power point

//In Solar application, the stage interfacing with the panel tries to regulate its input voltage driven by the panel
//This voltage is calle VpvRef (Note for other than solar application this could be any other parameter)
//The control of the power stage expects a reference in normalized Q24 format
 
//The following code sets the value of VpvRef_MPPT such that it tracks to the MPP
//Perturbation size for the VpvRef_MPPT is kept at 1% of Vmpp, where Vmpp is the MPP panel voltage at nominal operating ocnditions.
 

//The code below uses Gui_PanelPower variable which has been calculated 
//by multiplying the panel voltage and current drawn


    //Gui_PanelPower= _IQ6mpy(Gui_Vp_fb,((int16)Gui_IL_avg>>5));//Q6(16bit)*Q6(16bit) ==>IQMathLib==> Q6 (16bit)

    //Changed Gui Power from Q6 (above) to Q5 (below) to allow power calculation greater than 512Watt.
    //Use Q5 to display Gui_PanelPower on CCS watch window
    Gui_PanelPower= _IQ5mpy(((int16)Gui_Vp_fb>>1),((int16)Gui_IL_avg>>6));//Q5(16bit)*Q5(16bit) ==>IQMathLib==> Q5 (16bit)


// Following is the implementation using the solar lib function
    if(Run_MPPT==1)
    {
        // MPPT routine
        mppt_incc1.Ipv = IL_avg;//IpvRead_EMAVG; //IpvRead;
        mppt_incc1.Vpv = Vp_fb_Avg;//VpvRead_EMAVG; //VpvRead;

        mppt_incc_MACRO(mppt_incc1);

        VpvRef_MPPT = mppt_incc1.VmppOut;

        mppt_pno1.Ipv = IL_avg;//IpvRead_EMAVG; //IpvRead;
        mppt_pno1.Vpv = Vp_fb_Avg;//VpvRead_EMAVG; //VpvRead;

        mppt_pno_MACRO(mppt_pno1);

        //VpvRef_MPPT = mppt_pno1.VmppOut;

        if(VpvRef_MPPT<_IQ24(0.0))
        {
            VpvRef_MPPT=_IQ24(0.0);
        }
        else if(VpvRef_MPPT>_IQ24(0.9))
        {
            VpvRef_MPPT=_IQ24(0.9);
        }

        VpvRef=VpvRef_MPPT;

        Run_MPPT=0;
    }



//MPPT is a slow task, the following code enables to modulate the rate at which the MPPT is called

    if(MPPT_slew==0)
        {
            if(Enable_mppt==1)
            {
                Run_MPPT=1;
                mppt_incc1.mppt_enable=1;
            }
            MPPT_slew=500;
        }
    else
    {
            MPPT_slew--;
            mppt_incc1.mppt_enable=0;
    }

    //Disable MPPT when GPIO pin is active

//----------------------------------------------------------------------------------------------------------
//This section of code is used when the inverter board is connected to the DC-DC board


    if(inverter_connected == 1)//Inverter is connected. This is the default setting. The user must use the GUI
                                //to set this variable to 0 if the inverter is not connected//
    {
        /*if(GpioDataRegs.GPADAT.bit.GPIO16 == 1)//Check to see if the Inverter has pulled this pin HIGH
        {
            if(GPIO_cnt==5)//Inverter has pulled the GPIO pin HIGH indicating a fault. So wait for 5 count before disabling MPPT
            {

            GPIO_status=1;//Disable MPPT control when GPIO16 is pulled HIGH from the DC-AC Inverter side
            GPIO_cnt = 0;
            }
            GPIO_cnt++;
        }
        //****Check two conditions, "GPIO LOW" AND "COMMS up and running", before turning ON MPPT DC-DC.
        //When the COMMS run, the DC-DC uses the variable Gui_wLLCOutputVolt to know the LLC output volt
        //reported by the DC-AC Inverter, and this variable should be updated to reflect at least 200V

        if((GpioDataRegs.GPADAT.bit.GPIO16 == 0)&& (Gui_wLLCOutputVolt > 2000))//Check to see if the Inverter has pulled this pin LOW and COMMS is running
        {
            //Inverter has pulled the GPIO pin LOW
            //Lower LLC freq to 120kHz when the Inverter has pulled the GPIO pin LOW

            //PWM_ComplPairDB_CNF(3, period_LLC, 1, 0);
            if((*ePWM[3]).TBCTR < (((*ePWM[3]).TBPRD) - 10))
            {
                (*ePWM[3]).TBPRD = period_LLC-1;
                (*ePWM[3]).CMPA.half.CMPA = period_LLC/2;
            }

            GPIO_status=0;//Allow MPPT control by Enable_mppt when GPIO16 is pulled LOW
            GPIO_cnt = 0;
        }*/
    }

//---------------------------------------------------------------------------------------------------

    if(inverter_connected == 0)//Inverter not connected. The user must use the GUI to set
                               //inverter_connected = 0, implying that the inverter is not connected.
                               //This is required since the default setting is inverter_connected = 1
    {

        //Change the LLC PWM freq close to resonant freq of 120kHz;
        if((*ePWM[3]).TBCTR < (((*ePWM[3]).TBPRD) - 10))
        {
            (*ePWM[3]).TBPRD = period_LLC-1;
            (*ePWM[3]).CMPA.half.CMPA = period_LLC/2;
        }

        if((Start_DC_DC == 1) && (LLC_PWM_ON == 1))
        {
            if(GPIO_cnt==1000)
            {
            GPIO_status=0;//Allow MPPT control
            GPIO_cnt = 0;
            }
            GPIO_cnt++;
        }
        else
        {
            GPIO_status=1;//Disable MPPT control
            GPIO_cnt = 0;
        }
    }

//---------------------------------------------------------------------------------------------------------

    if(GPIO_status==1)//Keep MPPT disabled anytime GPIO_status is 1. This ensures MPPT is disabled
                        //during startup (when GPIO is an input pin and at high level), and under fault condition
        {
            Enable_mppt=0;   //Disable MPPT. This will turn off Boost PWM
            mppt_incc1.mppt_first=1;
            mppt_pno1.mppt_first=1;
            VpvRef=_IQ24(0.9);

            if(Operation_mode==1)//We also need to turn off LLC PWM to prevent OV condition at its output.
                            //But this cannot be done during start up since LLC has to be ON at that time.
                            //So we also check Operation_mode flag (Operation_mode=0=> startup mode,Operation_mode=1=>Run mode) before we turn off LLC PWM
            {
                EALLOW;
                EPwm3Regs.TZFRC.bit.OST = 1;//Turn off LLC PWM when in RUN mode (Operation_mode=1)
                EDIS;
                LLC_PWM_ON = 0;
                Operation_mode=2;//Go to restart mode, Operation_mode=2, and wait for restart command from inverter or the DC-DC GUI

            }
        }

            //************Code added to implement LLC output OV protection ONLY when the inverter is connected.
            //This parameter Gui_wLLCOutputVolt is available from the Inverter side when the Inverter is connected and
            //communicating to DC-DC. Therefore, LLC output will not have OV protection when the DC-DC runs without the Inverter
            /*if(Gui_wLLCOutputVolt > 4100)
            {
                EALLOW;
                EPwm3Regs.TZFRC.bit.OST = 1;//Turn off LLC PWM when LLC output is 410V
                EDIS;
            }*/

//------------------------------------------------------------------------------------------------------

    EPwm7Regs.ETCLR.bit.INT = 1;                    // Enable ePWM7 INTN pulse
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
    return;
}
//=========================================================================================================
//End of 20kHz ISR
//==========================================================================================================

//-----------------------------------------------------------------------------------------------------------
//RGB control function, each input takes 0-255 input to set brightness for the corresponding LED
void RGBWSetColor(Uint16 redDuty, Uint16 greenDuty, Uint16 blueDuty, Uint16 whiteDuty){
    (*ePWM[4]).CMPA.half.CMPA = redDuty << 6;
    (*ePWM[4]).CMPB = greenDuty << 6;
    (*ePWM[6]).CMPA.half.CMPA = blueDuty << 6;
    (*ePWM[6]).CMPB = whiteDuty << 6;
    return;
}
//-----------------------------------------------------------------------------------------------------------

void RGBModeControl(Uint16 DisplayMode){
    //static Uint16 fade;
    //static Uint16 fade_dir;
    switch(DisplayMode){
    case LED_W_BR:
        RGBWSetColor(0,0,0,fade);
        break;
    case LED_R_BR:
        RGBWSetColor(fade,0,0,0);
        break;
    case LED_G_BR:
        RGBWSetColor(0,fade,0,0);
        break;
    case LED_B_BR:
        RGBWSetColor(0,0,fade,0);
        break;
    case LED_ORG_BR:
        RGBWSetColor(fade>>1,fade>>3,0,0);
        break;
    case LED_CYA_BR:
        RGBWSetColor(0,fade>>1,fade>>1,0);
        break;
    case LED_MAG_BR:
        RGBWSetColor(fade>>1,0,fade>>1,0);
    case LED_YEL_BR:
        RGBWSetColor(fade>>1,fade>>1,0,0);
        break;
    case LED_R_FL:
        RGBWSetColor(7<<(fade&128),0,0,0);//Bitmap to flash 1x per direction
        break;
    }
    //Update fade counter
    if(!fade_dir){
        fade++;
    }
    else{
        fade--;
    }

    //Update fade direction
    if(fade == 255){
        fade_dir = 1;
    }
    else if(fade == 0){
        fade_dir = 0;
    }


}

//===========================================================================================================
//  STATE-MACHINE SEQUENCING AND SYNCRONIZATION
//===========================================================================================================

//--------------------------------- FRAMEWORK -------------------------------------
void A0(void)
{
    // loop rate synchronizer for A-tasks
    if(CpuTimer0Regs.TCR.bit.TIF == 1)
    {
        CpuTimer0Regs.TCR.bit.TIF = 1;  // clear flag

        //-----------------------------------------------------------
        (*A_Task_Ptr)();        // jump to an A Task (A1,A2,A3,...)
        //-----------------------------------------------------------

        VTimer0[0]++;           // virtual timer 0, instance 0 (spare)
    }

    Alpha_State_Ptr = &B0;      // Comment out to allow only A tasks
}

void B0(void)
{
    // loop rate synchronizer for B-tasks
    if(CpuTimer1Regs.TCR.bit.TIF == 1)
    {
        CpuTimer1Regs.TCR.bit.TIF = 1;              // clear flag

        //-----------------------------------------------------------
        (*B_Task_Ptr)();        // jump to a B Task (B1,B2,B3,...)
        //-----------------------------------------------------------
        VTimer1[0]++;           // virtual timer 1, instance 0 (spare)
    }

    Alpha_State_Ptr = &C0;      // Allow C state tasks
}

void C0(void)
{
    // loop rate synchronizer for C-tasks
    if(CpuTimer2Regs.TCR.bit.TIF == 1)
    {
        CpuTimer2Regs.TCR.bit.TIF = 1;              // clear flag

        //-----------------------------------------------------------
        (*C_Task_Ptr)();        // jump to a C Task (C1,C2,C3,...)
        //-----------------------------------------------------------
        VTimer2[0]++;           //virtual timer 2, instance 0 (spare)
    }

    Alpha_State_Ptr = &A0;  // Back to State A0
}


//=================================================================================
//  A - TASKS
//=================================================================================
//--------------------------------------------------------
void A1(void) 
//--------------------------------------------------------
{

    if (INCR_BUILD == 3)
    {
        //if(LedBlinkCnt==0)
         //{
            GpioDataRegs.GPACLEAR.bit.GPIO18 = 1; // Set Low to let the Inverter know that the DC-DC is ON (for BUILD 3)
         //}
         //else
         //{
         // GpioDataRegs.GPASET.bit.GPIO18 = 1;//Use for test only
         //}

        inverter_mode = inverter_connected;//Update inverter status (connected/not connected) only for GUI reporting//

        if((Operation_mode == 0) && (LLC_PWM_ON == 0))
        {
            EALLOW;
            EPwm3Regs.TZCLR.bit.OST = 1;//Clear TZ and turn on LLC PWM when in START UP mode
            EDIS;

            //Code modification below for SS**********
            //LLC_PWM_ON = 1;//Moved this LLC_PWM_ON flag setting(for inverter connected mode)after soft-start is complete
            if(inverter_connected == 0)
            {
            LLC_PWM_ON = 1;
            }
            //end SS mods******************************
        }

    }

    //-------------------
    //the next time CpuTimer0 'counter' reaches Period value go to A2
    A_Task_Ptr = &A2;
    //-------------------
}

//-----------------------------------------------------------------
void A2(void) 
//-----------------------------------------------------------------
{
//****************************************************
//Add soft-start for LLC, only for Build 3. Move on to A3 when soft-start is over. Otherwise go back to A1

    if((INCR_BUILD == 3) && (soft_start == 0))
    {
        //Soft-start is not complete
        if((*ePWM[3]).TBCTR < (((*ePWM[3]).TBPRD) - 10))
        {
            (*ePWM[3]).DBFED = dbred;
            (*ePWM[3]).DBRED = dbred;
        }

        if(dbred > 25)
        {
            if(db_dly_cntr == 100)
            {
                dbred = dbred-2;
                db_dly_cntr=0;
            }
            else
            {
                db_dly_cntr++;
            }
        }
        else
        {
            if((*ePWM[3]).TBCTR < (((*ePWM[3]).TBPRD) - 10))
            {
                (*ePWM[3]).DBFED = 25;
                (*ePWM[3]).DBRED = 25;
                LLC_PWM_ON = 1;//LLC stage is ON
                soft_start = 1;//Soft-start is complete
            }
         }

        A_Task_Ptr = &A1;
    }
    else
    {
        A_Task_Ptr = &A3;
    }
//End of soft-start code************************


    //-------------------
    //the next time CpuTimer0 'counter' reaches Period value go to A1

    //A_Task_Ptr = &A3;//Commented this to test soft-start
    //-------------------


}

//-----------------------------------------------------------------
void A3(void) 
//-----------------------------------------------------------------
{
    if(INCR_BUILD == 3)
    {
        //When the DC-DC runs without the Inverter, check for min panel voltage before starting MPPT DC-DC.
        //When DC-DC Runs with the Inverter connected, this flag (Start_DC_DC) is not used. Another GPIO (GPIO16)
        //enable signal is then used (activated by the Inverter) to start the DC-DC.
        if(Vp_fb_Avg >= VPV_MIN)
        {

            Start_DC_DC=1;//Start DC-DC MPPT provided LLC PWM is ON(This is checked in the 20kHz SECONDARY ISR)

            //***********************************************************************************
            //To control the variable "Start_DC_DC" from CCS watch window and run the DC-DC code with external 12V bias (not using PR798 bias supply)
            //COMMENT OUT the line above. This will allow code to run with MPPT off; Then the user will apply panel volt and start MPPT
            //from CCS watch window by setting Start_DC_DC = 1
            //************************************************************************************
        }
        else
        {
            Start_DC_DC = 0;//Do not start DC-DC MPPT
        }

        //if(GpioDataRegs.GPADAT.bit.GPIO16 == 0)
        if(GPIO_status == 0)
        {
            A_Task_Ptr = &A4;//Go to State A4 to turn ON MPPT
        }
        else
        {
        //-------------------
        //the next time CpuTimer0 'counter' reaches Period value go to A1
        A_Task_Ptr = &A1;
        //-------------------
        }

    }

    A_Task_Ptr = &A4;
}

//-----------------------------------------------------------------
void A4(void) 
//-----------------------------------------------------------------
{
    if(INCR_BUILD == 3)
    {

        //inverter_mode = inverter_connected;

        //if(GpioDataRegs.GPADAT.bit.GPIO16 == 0)
        if(GPIO_status == 0)
        {
            Enable_mppt=1;   //Enable MPPT.
            Operation_mode=1;
            A_Task_Ptr = &A1;
        }
    }

    //-------------------
    //the next time CpuTimer0 'counter' reaches Period value go to A1
    A_Task_Ptr = &A1;
    //-------------------

}

//=================================================================================
//  B - TASKS
//=================================================================================

//----------------------------------- USER ----------------------------------------

//----------------------------------------
void B1(void)
//----------------------------------------
{
    HistPtr++;
    if (HistPtr >= HistorySize)
        HistPtr = 0;

    // BoxCar Averages - Input Raw samples into BoxCar arrays
    //----------------------------------------------------------------
    //Hist_IL[HistPtr] = ILR;
    Hist_IL[HistPtr] = IL_avg >> 12;//IL_avg is in Q24. So convert to Q12
    Hist_Vb_fb[HistPtr] = Vb_fbR;
    Hist_Vp_fb[HistPtr] = Vp_fbR;

    // Measurements
    //----------------------------------------------------------------
    // view following variables in Watch Window as:
    //      Gui_IL = Q??
    //      Gui_Vb_fb = Q??
    //      Gui_Vp_fb = Q??

    //Average Boost Voltage Calculation
    temp_Scratch=0;
    for(i=0; i<HistorySize; i++)
        temp_Scratch = temp_Scratch + Hist_Vb_fb[i];    //Q12 * 8 = Q15
    Gui_Vb_fb = ((long)temp_Scratch*(long)K_Vb_fb) >> 15;   //Q15*Q15 >> 15 = Q15

    //Average Panel Voltage Calculation
    temp_Scratch=0;
    for(i=0; i<HistorySize; i++)
        temp_Scratch = temp_Scratch + Hist_Vp_fb[i];    //Q12 * 8 = Q15
    Gui_Vp_fb = ((long)temp_Scratch*(long)K_Vp_fb) >> 15;   //Q15*Q15 >> 15 = Q15
    
    //Average Cuurent calculation
    temp_Scratch=0;
    for(i=0; i<HistorySize; i++)
        temp_Scratch = temp_Scratch + Hist_IL[i];   //Q12 * 8 = Q15
    Gui_IL_avg = ((long)temp_Scratch*(long)K_IL_avg) >> 15; //Q15*Q15 >> 15 = Q15


    struct ECAN_REGS ECanaShadow;

    //Send Panel Input Voltage, Current, Power
    ECanaMboxes.MBOX0.MDL.all = ((unsigned long)Gui_Vb_fb << 16) + (Uint16)Gui_Vp_fb;     //byte3,byte2,byte1,byte0
    ECanaMboxes.MBOX0.MDH.all = ((unsigned long)Gui_PanelPower << 16) + (Uint16)Gui_IL_avg; //byte7,byte6,byte5,byte4

    //Send Operation mode, thermal probes
    ECanaMboxes.MBOX1.MDL.all = ((unsigned long)HTSK_TP << 16) + (Uint16)Operation_mode;     //byte3,byte2,byte1,byte0
    ECanaMboxes.MBOX1.MDH.all = EXTHTSK_TP;                                                 //byte7,byte6,byte5,byte4

    ECanaShadow.CANTRS.all = 0;
    ECanaShadow.CANTRS.bit.TRS0 = 1;     // Set TRS for transmitting mailbox
    ECanaShadow.CANTRS.bit.TRS1 = 1;     // Set TRS for transmitting mailbox
    ECanaRegs.CANTRS.all = ECanaShadow.CANTRS.all;


    //-----------------
    //the next time CpuTimer1 'counter' reaches Period value go to B2
    B_Task_Ptr = &B2;
    //-----------------
}

//----------------------------------------
void B2(void) // Blink LED on the control CArd
//----------------------------------------
{
    struct ECAN_REGS ECanaShadow;


        //Check acknowledgment of CAN messages
        if(ECanaRegs.CANTA.bit.TA0 == 0 ){                  //Not acknowledged
            if(CAN_noACK < CAN_MAX_UNACK+10)CAN_noACK++;
            ECanaShadow.CANTA.all = ECanaRegs.CANTA.all;
        }
        else{
            CAN_noACK = 0;
            ECanaShadow.CANTA.all = 0;
            ECanaShadow.CANTA.bit.TA0 = 1;                  // Clear TA5
            ECanaShadow.CANTA.bit.TA1 = 1;
            ECanaRegs.CANTA.all = ECanaShadow.CANTA.all;
        }

        if(ECanaShadow.CANRMP.bit.RMP2 == 1){
            ECanaShadow.CANRMP.bit.RMP2 = 0;
            LedBlinkCnt = 16383;
        }


    if(LedBlinkCnt==0)
        {
            //GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1;    //turn on/off LD3 on the controlCARD
            LedBlinkCnt=5;
        }
    else
            LedBlinkCnt--;

    //-----------------
    //the next time CpuTimer1 'counter' reaches Period value go to B1
    B_Task_Ptr = &B1;
    //-----------------
}


//=================================================================================
//  C - TASKS
//=================================================================================

//--------------------------------- USER ------------------------------------------

//------------------------------------------------------
void C1(void)  // LED Status Control
//------------------------------------------------------
{


    if(Operation_mode == 0){
        if(CAN_noACK < CAN_MAX_UNACK) ledSetMode = LED_W_BR;
        else ledSetMode = LED_B_BR;
    }
    else if(Operation_mode == 1){
        if (HTSK_TP > HTSK_OHVAL) ledSetMode = LED_MAG_BR;
        else if(CAN_noACK >= CAN_MAX_UNACK) ledSetMode = LED_G_BR;
        else ledSetMode = LED_CYA_BR;
    }
    else if(Operation_mode == 2){
        if (HTSK_TP > HTSK_OHVAL) ledSetMode = LED_R_FL;
        else if(CAN_noACK < CAN_MAX_UNACK) ledSetMode = LED_ORG_BR;
        else ledSetMode = LED_R_BR;
    }

    RGBModeControl(ledSetMode);

    //-----------------
    //the next time CpuTimer2 'counter' reaches Period value go to C2
    C_Task_Ptr = &C1;
    //-----------------
}
//----------------------------------------
void C2(void) //
//----------------------------------------
{
    //-----------------
    //the next time CpuTimer2 'counter' reaches Period value go to C1
        C_Task_Ptr = &C1;
    //-----------------
}


//-----------------------------------------
void C3(void) // 
//-----------------------------------------
{
    //-----------------
    //the next time CpuTimer2 'counter' reaches Period value go to C1
        C_Task_Ptr = &C1;
    //-----------------

}
