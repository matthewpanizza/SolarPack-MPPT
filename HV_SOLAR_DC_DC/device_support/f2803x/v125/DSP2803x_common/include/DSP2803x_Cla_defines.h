//###########################################################################
//
// FILE:   DSP2803x_Cla_defines.h
//
// TITLE:  #defines used in CLA examples
//
//###########################################################################
// $TI Release: F2803x C/C++ Header Files and Peripheral Examples V1.25 $
// $Release Date: August 26, 2011 $
//###########################################################################

#ifndef DSP2803X_CLA_DEFINES_H
#define DSP2803X_CLA_DEFINES_H

#ifdef __cplusplus
extern "C" {
#endif

// MCTL Register
//==========================
#define CLA_FORCE_RESET           0x1
#define CLA_IACK_ENABLE           0x1
#define CLA_IACK_DISABLE          0x0

// MMEMCFG Register
//==========================
#define CLA_CLA_SPACE             0x1
#define CLA_CPU_SPACE             0x0

// MIER Interrupt Enable Register
//==========================
#define CLA_INT_ENABLE            0x1
#define CLA_INT_DISABLE           0x0

// PERINT11SEL Interrupt Source Select Register
//================================
#define CLA_INT1_NONE             0x1
#define CLA_INT1_ADCINT1          0x0
#define CLA_INT1_EPWM1INT         0x2

#define CLA_INT2_NONE             0x1
#define CLA_INT2_ADCINT2          0x0
#define CLA_INT2_EPWM2INT         0x2

#define CLA_INT3_NONE             0x1
#define CLA_INT3_ADCINT3          0x0
#define CLA_INT3_EPWM3INT         0x2

#define CLA_INT4_NONE             0x1
#define CLA_INT4_ADCINT4          0x0
#define CLA_INT4_EPWM4INT         0x2

#define CLA_INT5_NONE             0x1
#define CLA_INT5_ADCINT5          0x0
#define CLA_INT5_EPWM5INT         0x2

#define CLA_INT6_NONE             0x1
#define CLA_INT6_ADCINT6          0x0
#define CLA_INT6_EPWM6INT         0x2

#define CLA_INT7_NONE             0x1
#define CLA_INT7_ADCINT7          0x0
#define CLA_INT7_EPWM7INT         0x2

#define CLA_INT8_NONE             0x1
#define CLA_INT8_ADCINT4          0x0
#define CLA_INT8_CPUTIMER0INT     0x2


//---------------------------------------------------------------------------
// Useful CLA Operation Macros:
//================================

#define Cla1ForceTask1andWait() asm("  IACK  #0x0001");             \
                                asm("  RPT #3 || NOP");             \
                                while(Cla1Regs.MIRUN.bit.INT1 == 1);


#define Cla1ForceTask2andWait() asm("  IACK  #0x0002");             \
                                asm("  RPT #3 || NOP");             \
                                while(Cla1Regs.MIRUN.bit.INT2 == 1);

#define Cla1ForceTask3andWait() asm("  IACK  #0x0004");             \
                                asm("  RPT #3 || NOP");             \
                                while(Cla1Regs.MIRUN.bit.INT3 == 1);

#define Cla1ForceTask4andWait() asm("  IACK  #0x0008");             \
                                asm("  RPT #3 || NOP");             \
                                while(Cla1Regs.MIRUN.bit.INT4 == 1);

#define Cla1ForceTask5andWait() asm("  IACK  #0x0010");             \
                                asm("  RPT #3 || NOP");             \
                                while(Cla1Regs.MIRUN.bit.INT5 == 1);

#define Cla1ForceTask6andWait() asm("  IACK  #0x0020");             \
                                asm("  RPT #3 || NOP");             \
                                while(Cla1Regs.MIRUN.bit.INT6 == 1);

#define Cla1ForceTask7andWait() asm("  IACK  #0x0040");             \
                                asm("  RPT #3 || NOP");             \
                                while(Cla1Regs.MIRUN.bit.INT7 == 1);

#define Cla1ForceTask8andWait() asm("  IACK  #0x0080");             \
                                asm("  RPT #3 || NOP");             \
                                while(Cla1Regs.MIRUN.bit.INT8 == 1);


#define Cla1ForceTask1()        asm("  IACK  #0x0001")
#define Cla1ForceTask2()        asm("  IACK  #0x0002")
#define Cla1ForceTask3()        asm("  IACK  #0x0004")
#define Cla1ForceTask4()        asm("  IACK  #0x0008")
#define Cla1ForceTask5()        asm("  IACK  #0x0010")
#define Cla1ForceTask6()        asm("  IACK  #0x0020")
#define Cla1ForceTask7()        asm("  IACK  #0x0040")
#define Cla1ForceTask8()        asm("  IACK  #0x0080")

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif   // - end of DSP2803x_CLA_DEFINES_H

//===========================================================================
// End of file.
//===========================================================================
