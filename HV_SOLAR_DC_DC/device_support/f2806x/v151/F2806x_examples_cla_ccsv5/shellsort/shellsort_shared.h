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

#ifndef SHELLSORT_SHARED_H_
#define SHELLSORT_SHARED_H_
#ifdef __cplusplus
extern "C" {
#endif

//Task 1 (C) Variables
#define LENGTH1 (int32)((sizeof(vector1)/sizeof(vector1[0])))
extern float32 vector1[5];
extern float32 vector1_sorted[];

//Task 2 (C) Variables
#define LENGTH2 (int32)((sizeof(vector2)/sizeof(vector2[0])))
extern float32 vector2[10];
extern float32 vector2_sorted[];

//Task 3 (C) Variables
#define LENGTH3 (int32)((sizeof(vector3)/sizeof(vector2[0])))
extern int32   vector3[9];
extern int32   vector3_sorted[];

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
#endif /*SHELLSORT_SHARED_H_*/
