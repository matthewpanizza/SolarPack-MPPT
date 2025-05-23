//###########################################################################
// Description:
//
// Declare shared memory variables and assign them to specific CLA-accessible
// memory locations
//
//! \addtogroup f2806x_example_cla_list
//! \b Memory \b Allocation \n
//!  - CLA1 to CPU Message RAM
//!    - fDet - Determinant of the 3x3 matrix
//!  - CPU to CLA1 Message RAM
//!    - x - 3x3 input matrix
//
//###########################################################################
// $TI Release: F2806x C/C++ Header Files and Peripheral Examples V151 $
// $Release Date: February  2, 2016 $
// $Copyright: Copyright (C) 2011-2016 Texas Instruments Incorporated -
//             http://www.ti.com/ ALL RIGHTS RESERVED $
//###########################################################################

#include "DSP28x_Project.h"
// Include the test header file whose name is based on the test name
// which is defined by the macro TEST on the command line
#include XSTRINGIZE(XCONCAT(TEST_NAME,_shared.h))


//GLobal Data
 //Ensure that all data is placed in the data rams

//Task 1 (C) Variables
#pragma DATA_SECTION(x,"CpuToCla1MsgRAM")
float x[M][N];
#pragma DATA_SECTION(fDet,"Cla1ToCpuMsgRAM")
float fDet;
float z[BUFFER_SIZE];


//Task 2 (C) Variables

//Task 3 (C) Variables

//Task 4 (C) Variables

//Task 5 (C) Variables

//Task 6 (C) Variables

//Task 7 (C) Variables

//Task 8 (C) Variables

//Common (C) Variables


