//###########################################################################
// Description:
//
// Declare shared variables
//
//###########################################################################
// $TI Release: F2806x C/C++ Header Files and Peripheral Examples V151 $
// $Release Date: February  2, 2016 $
// $Copyright: Copyright (C) 2011-2016 Texas Instruments Incorporated -
//             http://www.ti.com/ ALL RIGHTS RESERVED $
//###########################################################################

#ifndef ACOS_SHARED_H_
#define ACOS_SHARED_H_

#ifdef __cplusplus
extern "C" {
#endif

//Task 1 (C) Variables
#define BUFFER_SIZE 	64
#define TABLE_SIZE		64
#define TABLE_SIZE_M_1	TABLE_SIZE-1
#define PIBYTWO			1.570796327
#define PI				3.141592653589
#define INV2PI			0.159154943
extern float y[];
extern float fVal;
extern float fResult;
extern float CLAacosinTable[];

//Task 2 (C) Variables

//Task 3 (C) Variables

//Task 4 (C) Variables

//Task 5 (C) Variables

//Task 6 (C) Variables

//Task 7 (C) Variables

//Task 8 (C) Variables

//Common (C) Variables

//CLA C Tasks
__interrupt void Cla1Task1();
__interrupt void Cla1Task2();
__interrupt void Cla1Task3();
__interrupt void Cla1Task4();
__interrupt void Cla1Task5();
__interrupt void Cla1Task6();
__interrupt void Cla1Task7();
__interrupt void Cla1Task8();

//C Function Prototypes
void test_run();
void test_report();

#ifdef __cplusplus
}
#endif /* extern "C" */
#endif /*ACOS_SHARED_H_*/
