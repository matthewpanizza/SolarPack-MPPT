//###########################################################################
//
// FILE:   usb_host_msc.c
//
// TITLE:  USB Mass Storage Class Host example. 
//
// ASSUMPTIONS:
//
//    This program requires:
//        - the F2806x header files 
//        - Driverlib (/ControlSUITE/lib/MWare/f2806x/driverlib/)
//        - Usblib (/ControlSUITE/lib/MWare/f2806x/usblib/)
//        - FatFS (/ControlSUITE/lib/MWare/f2806x/third_party/fatfs/)
//        - USB Capable F2806x
//
// Description:
//
//! \addtogroup f2806x_example_list
//! <h1>USB Mass Storage Class Host (usb_host_msc)</h1>
//!
//! This example application demonstrates reading a file system from a USB mass
//! storage class device.  It makes use of FatFs, a FAT file system driver.  It
//! provides a simple command console via the UART for issuing commands to view
//! and navigate the file system on the mass storage device.
//!
//! The first UART, which is connected to the FTDI virtual serial port on the
//! evaluation board, is configured for 115,200 bits per second, and 8-N-1
//! mode.  When the program is started a message will be printed to the
//! terminal.  Type ``help'' for command help.
//!
//! For additional details about FatFs, see the following site:
//! http://elm-chan.org/fsw/ff/00index_e.html
//
//  Section .ebss was initialized to 0x0000 in
//  F2806x_CodeStartBranch_ebss_init.asm. This was to ensure that all global
//  variables of the usblib are initialized to 0x0000 on the F2806x.
//		
//###########################################################################	
// $TI Release: F2806x C/C++ Header Files and Peripheral Examples V151 $ 
// $Release Date: February  2, 2016 $ 
//#############################################################################

#include "DSP28x_Project.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/uart.h"
#include "driverlib/usb.h"
#include "inc/hw_usb.h"
#include "usblib/usblib.h"
#include "driverlib/usb_hal.h"
#include "usblib/usbmsc.h"
#include "usblib/host/usbhost.h"
#include "usblib/host/usbhmsc.h"
#include "utils/cmdline.h"
#include "utils/uartstdio.h"
#include "third_party/fatfs/src/ff.h"

//*****************************************************************************
//
// Defines the size of the buffers that hold the path, or temporary data from
// the memory card.  There are two buffers allocated of this size.  The buffer
// size must be large enough to hold the longest expected full path name,
// including the file name, and a trailing null character.
//
//*****************************************************************************
#define PATH_BUF_SIZE   80

//*****************************************************************************
//
// Defines the size of the buffer that holds the command line.
//
//*****************************************************************************
#define CMD_BUF_SIZE    64

//*****************************************************************************
//
// This buffer holds the full path to the current working directory.  Initially
// it is root ("/").
//
//*****************************************************************************
static char g_cCwdBuf[PATH_BUF_SIZE] = "/";

//*****************************************************************************
//
// A temporary data buffer used when manipulating file paths, or reading data
// from the memory card.
//
//*****************************************************************************
static char g_cTmpBuf[PATH_BUF_SIZE];

//*****************************************************************************
//
// The buffer that holds the command line.
//
//*****************************************************************************
static char g_cCmdBuf[CMD_BUF_SIZE];

//*****************************************************************************
//
// Current FAT fs state.
//
//*****************************************************************************
static FATFS g_sFatFs;
static DIR g_sDirObject;
static FILINFO g_sFileInfo;
static FIL g_sFileObject;

//*****************************************************************************
//
// A structure that holds a mapping between an FRESULT numerical code,
// and a string representation.  FRESULT codes are returned from the FatFs
// FAT file system driver.
//
//*****************************************************************************
typedef struct
{
    FRESULT fresult;
    char *pcResultStr;
}
tFresultString;

//*****************************************************************************
//
// A macro to make it easy to add result codes to the table.
//
//*****************************************************************************
#define FRESULT_ENTRY(f)        { (f), (#f) }

//*****************************************************************************
//
// A table that holds a mapping between the numerical FRESULT code and
// it's name as a string.  This is used for looking up error codes for
// printing to the console.
//
//*****************************************************************************
tFresultString g_sFresultStrings[] =
{
    FRESULT_ENTRY(FR_OK),
    FRESULT_ENTRY(FR_NOT_READY),
    FRESULT_ENTRY(FR_NO_FILE),
    FRESULT_ENTRY(FR_NO_PATH),
    FRESULT_ENTRY(FR_INVALID_NAME),
    FRESULT_ENTRY(FR_INVALID_DRIVE),
    FRESULT_ENTRY(FR_DENIED),
    FRESULT_ENTRY(FR_EXIST),
    FRESULT_ENTRY(FR_RW_ERROR),
    FRESULT_ENTRY(FR_WRITE_PROTECTED),
    FRESULT_ENTRY(FR_NOT_ENABLED),
    FRESULT_ENTRY(FR_NO_FILESYSTEM),
    FRESULT_ENTRY(FR_INVALID_OBJECT),
    FRESULT_ENTRY(FR_MKFS_ABORTED)
};

//*****************************************************************************
//
// A macro that holds the number of result codes.
//
//*****************************************************************************
#define NUM_FRESULT_CODES (sizeof(g_sFresultStrings) / sizeof(tFresultString))

//*****************************************************************************
//
// The size of the host controller's memory pool in bytes.
//
//*****************************************************************************
#define HCD_MEMORY_SIZE         128

//*****************************************************************************
//
// The memory pool to provide to the Host controller driver.
//
//*****************************************************************************
unsigned char g_pHCDPool[HCD_MEMORY_SIZE];

//*****************************************************************************
//
// The instance data for the MSC driver.
//
//*****************************************************************************
tUSBHMSCInstance *g_psMSCInstance = 0;

//*****************************************************************************
//
// Declare the USB Events driver interface.
//
//*****************************************************************************
DECLARE_EVENT_DRIVER(g_sUSBEventDriver, 0, 0, USBHCDEvents);

//*****************************************************************************
//
// The global that holds all of the host drivers in use in the application.
// In this case, only the MSC class is loaded.
//
//*****************************************************************************
static tUSBHostClassDriver const * const g_ppHostClassDrivers[] =
{
    &g_sUSBHostMSCClassDriver,
    &g_sUSBEventDriver
};

//*****************************************************************************
//
// This global holds the number of class drivers in the g_ppHostClassDrivers
// list.
//
//*****************************************************************************
#define NUM_CLASS_DRIVERS       (sizeof(g_ppHostClassDrivers) /               \
                                 sizeof(g_ppHostClassDrivers[0]))

//*****************************************************************************
//
// Hold the current state for the application.
//
//*****************************************************************************
typedef enum
{
    //
    // No device is present.
    //
    STATE_NO_DEVICE,

    //
    // Mass storage device is being enumerated.
    //
    STATE_DEVICE_ENUM,

    //
    // Mass storage device is ready.
    //
    STATE_DEVICE_READY,

    //
    // An unsupported device has been attached.
    //
    STATE_UNKNOWN_DEVICE,

    //
    // A power fault has occurred.
    //
    STATE_POWER_FAULT
}
tState;
volatile tState g_eState;
volatile tState g_eUIState;

//*****************************************************************************
//
// The current USB operating mode - Host, Device or unknown.
//
//*****************************************************************************
tUSBMode g_eCurrentUSBMode;

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, unsigned long ulLine)
{
	__asm(" ESTOP0");
	while(1)
	{
	}
}
#endif

//*****************************************************************************
//
// USB Mode callback
//
// \param ulIndex is the zero-based index of the USB controller making the
//        callback.
// \param eMode indicates the new operating mode.
//
// This function is called by the USB library whenever an OTG mode change
// occurs and, if a connection has been made, informs us of whether we are to
// operate as a host or device.
//
// \return None.
//
//*****************************************************************************
void
ModeCallback(unsigned long ulIndex, tUSBMode eMode)
{
    //
    // Save the new mode.
    //

    g_eCurrentUSBMode = eMode;
}

//*****************************************************************************
//
// This function returns a string representation of an error code that was
// returned from a function call to FatFs.  It can be used for printing human
// readable error messages.
//
//*****************************************************************************
const char *
StringFromFresult(FRESULT fresult)
{
    unsigned int uIdx;

    //
    // Enter a loop to search the error code table for a matching error code.
    //
    for(uIdx = 0; uIdx < NUM_FRESULT_CODES; uIdx++)
    {
        //
        // If a match is found, then return the string name of the error code.
        //
        if(g_sFresultStrings[uIdx].fresult == fresult)
        {
            return(g_sFresultStrings[uIdx].pcResultStr);
        }
    }

    //
    // At this point no matching code was found, so return a string indicating
    // unknown error.
    //
    return("UNKNOWN ERROR CODE");
}

//*****************************************************************************
//
// This function implements the "ls" command.  It opens the current directory
// and enumerates through the contents, and prints a line for each item it
// finds.  It shows details such as file attributes, time and date, and the
// file size, along with the name.  It shows a summary of file sizes at the end
// along with free space.
//
//*****************************************************************************
int
Cmd_ls(int argc, char *argv[])
{
    unsigned long ulTotalSize;
    unsigned long ulFileCount;
    unsigned long ulDirCount;
    FRESULT fresult;
    FATFS *pFatFs;

    //
    // Do not attempt to do anything if there is not a drive attached.
    //
    if(g_eState != STATE_DEVICE_READY)
    {
        return(FR_NOT_READY);
    }

    //
    // Open the current directory for access.
    //
    fresult = f_opendir(&g_sDirObject, g_cCwdBuf);

    //
    // Check for error and return if there is a problem.
    //
    if(fresult != FR_OK)
    {
        return(fresult);
    }

    ulTotalSize = 0;
    ulFileCount = 0;
    ulDirCount = 0;

    //
    // Enter loop to enumerate through all directory entries.
    //
    while(1)
    {
        //
        // Read an entry from the directory.
        //
        fresult = f_readdir(&g_sDirObject, &g_sFileInfo);

        //
        // Check for error and return if there is a problem.
        //
        if(fresult != FR_OK)
        {
            return(fresult);
        }

        //
        // If the file name is blank, then this is the end of the listing.
        //
        if(!g_sFileInfo.fname[0])
        {
            break;
        }

        //
        // If the attribute is directory, then increment the directory count.
        //
        if(g_sFileInfo.fattrib & AM_DIR)
        {
            ulDirCount++;
        }

        //
        // Otherwise, it is a file.  Increment the file count, and add in the
        // file size to the total.
        //
        else
        {
            ulFileCount++;
            ulTotalSize += g_sFileInfo.fsize;
        }

        //
        // Print the entry information on a single line with formatting to show
        // the attributes, date, time, size, and name.
        //
        UARTprintf("%c%c%c%c%c %u/%02u/%02u %02u:%02u %9u  %s\n",
                   (g_sFileInfo.fattrib & AM_DIR) ? 'D' : '-',
                   (g_sFileInfo.fattrib & AM_RDO) ? 'R' : '-',
                   (g_sFileInfo.fattrib & AM_HID) ? 'H' : '-',
                   (g_sFileInfo.fattrib & AM_SYS) ? 'S' : '-',
                   (g_sFileInfo.fattrib & AM_ARC) ? 'A' : '-',
                   (g_sFileInfo.fdate >> 9) + 1980,
                   (g_sFileInfo.fdate >> 5) & 15,
                   g_sFileInfo.fdate & 31,
                   (g_sFileInfo.ftime >> 11),
                   (g_sFileInfo.ftime >> 5) & 63,
                   g_sFileInfo.fsize,
                   g_sFileInfo.fname);
    }

    //
    // Print summary lines showing the file, dir, and size totals.
    //
    UARTprintf("\n%4u File(s),%10u bytes total\n%4u Dir(s)",
               ulFileCount, ulTotalSize, ulDirCount);

    //
    // Get the free space.
    //
    fresult = f_getfree("/", &ulTotalSize, &pFatFs);

    //
    // Check for error and return if there is a problem.
    //
    if(fresult != FR_OK)
    {
        return(fresult);
    }

    //
    // Display the amount of free space that was calculated.
    //
    UARTprintf(", %10uK bytes free\n", ulTotalSize * pFatFs->sects_clust / 2);

    //
    // Made it to here, return with no errors.
    //
    return(0);
}

//*****************************************************************************
//
// This function implements the "cd" command.  It takes an argument that
// specifies the directory to make the current working directory.  Path
// separators must use a forward slash "/".  The argument to cd can be one of
// the following:
//
// * root ("/")
// * a fully specified path ("/my/path/to/mydir")
// * a single directory name that is in the current directory ("mydir")
// * parent directory ("..")
//
// It does not understand relative paths, so don't try something like this:
// ("../my/new/path")
//
// Once the new directory is specified, it attempts to open the directory to
// make sure it exists.  If the new path is opened successfully, then the
// current working directory (cwd) is changed to the new path.
//
//*****************************************************************************
int
Cmd_cd(int argc, char *argv[])
{
    unsigned int uIdx;
    FRESULT fresult;

    //
    // Do not attempt to do anything if there is not a drive attached.
    //
    if(g_eState != STATE_DEVICE_READY)
    {
        return(FR_NOT_READY);
    }

    //
    // Copy the current working path into a temporary buffer so it can be
    // manipulated.
    //
    strcpy(g_cTmpBuf, g_cCwdBuf);

    //
    // If the first character is /, then this is a fully specified path, and it
    // should just be used as-is.
    //
    if(argv[1][0] == '/')
    {
        //
        // Make sure the new path is not bigger than the cwd buffer.
        //
        if(strlen(argv[1]) + 1 > sizeof(g_cCwdBuf))
        {
            UARTprintf("Resulting path name is too long\n");
            return(0);
        }

        //
        // If the new path name (in argv[1])  is not too long, then copy it
        // into the temporary buffer so it can be checked.
        //
        else
        {
            strncpy(g_cTmpBuf, argv[1], sizeof(g_cTmpBuf));
        }
    }

    //
    // If the argument is .. then attempt to remove the lowest level on the
    // CWD.
    //
    else if(!strcmp(argv[1], ".."))
    {
        //
        // Get the index to the last character in the current path.
        //
        uIdx = strlen(g_cTmpBuf) - 1;

        //
        // Back up from the end of the path name until a separator (/) is
        // found, or until we bump up to the start of the path.
        //
        while((g_cTmpBuf[uIdx] != '/') && (uIdx > 1))
        {
            //
            // Back up one character.
            //
            uIdx--;
        }

        //
        // Now we are either at the lowest level separator in the current path,
        // or at the beginning of the string (root).  So set the new end of
        // string here, effectively removing that last part of the path.
        //
        g_cTmpBuf[uIdx] = 0;
    }

    //
    // Otherwise this is just a normal path name from the current directory,
    // and it needs to be appended to the current path.
    //
    else
    {
        //
        // Test to make sure that when the new additional path is added on to
        // the current path, there is room in the buffer for the full new path.
        // It needs to include a new separator, and a trailing null character.
        //
        if(strlen(g_cTmpBuf) + strlen(argv[1]) + 1 + 1 > sizeof(g_cCwdBuf))
        {
            UARTprintf("Resulting path name is too long\n");
            return(0);
        }

        //
        // The new path is okay, so add the separator and then append the new
        // directory to the path.
        //
        else
        {
            //
            // If not already at the root level, then append a /
            //
            if(strcmp(g_cTmpBuf, "/"))
            {
                strcat(g_cTmpBuf, "/");
            }

            //
            // Append the new directory to the path.
            //
            strcat(g_cTmpBuf, argv[1]);
        }
    }

    //
    // At this point, a candidate new directory path is in chTmpBuf.  Try to
    // open it to make sure it is valid.
    //
    fresult = f_opendir(&g_sDirObject, g_cTmpBuf);

    //
    // If it can't be opened, then it is a bad path.  Inform user and return.
    //
    if(fresult != FR_OK)
    {
        UARTprintf("cd: %s\n", g_cTmpBuf);
        return(fresult);
    }

    //
    // Otherwise, it is a valid new path, so copy it into the CWD.
    //
    else
    {
        strncpy(g_cCwdBuf, g_cTmpBuf, sizeof(g_cCwdBuf));
    }

    //
    // Return success.
    //
    return(0);
}

//*****************************************************************************
//
// This function implements the "pwd" command.  It simply prints the current
// working directory.
//
//*****************************************************************************
int
Cmd_pwd(int argc, char *argv[])
{
    //
    // Do not attempt to do anything if there is not a drive attached.
    //
    if(g_eState != STATE_DEVICE_READY)
    {
        return(FR_NOT_READY);
    }

    //
    // Print the CWD to the console.
    //
    UARTprintf("%s\n", g_cCwdBuf);

    //
    // Return success.
    //
    return(0);
}

//*****************************************************************************
//
// This function implements the "cat" command.  It reads the contents of a file
// and prints it to the console.  This should only be used on text files.  If
// it is used on a binary file, then a bunch of garbage is likely to printed on
// the console.
//
//*****************************************************************************
int
Cmd_cat(int argc, char *argv[])
{
    FRESULT fresult;
    unsigned short usBytesRead;

    //
    // Do not attempt to do anything if there is not a drive attached.
    //
    if(g_eState != STATE_DEVICE_READY)
    {
        return(FR_NOT_READY);
    }

    //
    // First, check to make sure that the current path (CWD), plus the file
    // name, plus a separator and trailing null, will all fit in the temporary
    // buffer that will be used to hold the file name.  The file name must be
    // fully specified, with path, to FatFs.
    //
    if(strlen(g_cCwdBuf) + strlen(argv[1]) + 1 + 1 > sizeof(g_cTmpBuf))
    {
        UARTprintf("Resulting path name is too long\n");
        return(0);
    }

    //
    // Copy the current path to the temporary buffer so it can be manipulated.
    //
    strcpy(g_cTmpBuf, g_cCwdBuf);

    //
    // If not already at the root level, then append a separator.
    //
    if(strcmp("/", g_cCwdBuf))
    {
        strcat(g_cTmpBuf, "/");
    }

    //
    // Now finally, append the file name to result in a fully specified file.
    //
    strcat(g_cTmpBuf, argv[1]);

    //
    // Open the file for reading.
    //
    fresult = f_open(&g_sFileObject, g_cTmpBuf, FA_READ);

    //
    // If there was some problem opening the file, then return an error.
    //
    if(fresult != FR_OK)
    {
        return(fresult);
    }

    //
    // Enter a loop to repeatedly read data from the file and display it, until
    // the end of the file is reached.
    //
    do
    {
        //
        // Read a block of data from the file.  Read as much as can fit in the
        // temporary buffer, including a space for the trailing null.
        //
        fresult = f_read(&g_sFileObject, g_cTmpBuf, sizeof(g_cTmpBuf) - 1,
                         &usBytesRead);

        //
        // If there was an error reading, then print a newline and return the
        // error to the user.
        //
        if(fresult != FR_OK)
        {
            UARTprintf("\n");
            return(fresult);
        }

        //
        // Null terminate the last block that was read to make it a null
        // terminated string that can be used with printf.
        //
        g_cTmpBuf[usBytesRead] = 0;

        //
        // Print the last chunk of the file that was received.
        //
        UARTprintf("%s", g_cTmpBuf);

        //
        // Continue reading until less than the full number of bytes are read.
        // That means the end of the buffer was reached.
        //
    }
    while(usBytesRead == sizeof(g_cTmpBuf) - 1);

    //
    // Return success.
    //
    return(0);
}

//*****************************************************************************
//
// This function implements the "help" command.  It prints a simple list of the
// available commands with a brief description.
//
//*****************************************************************************
int
Cmd_help(int argc, char *argv[])
{
    tCmdLineEntry *pEntry;

    //
    // Print some header text.
    //
    UARTprintf("\nAvailable commands\n");
    UARTprintf("------------------\n");

    //
    // Point at the beginning of the command table.
    //
    pEntry = &g_sCmdTable[0];

    //
    // Enter a loop to read each entry from the command table.  The end of the
    // table has been reached when the command name is NULL.
    //
    while(pEntry->pcCmd)
    {
        //
        // Print the command name and the brief description.
        //
        UARTprintf("%s%s\n", pEntry->pcCmd, pEntry->pcHelp);

        //
        // Advance to the next entry in the table.
        //
        pEntry++;
    }

    //
    // Return success.
    //
    return(0);
}

//*****************************************************************************
//
// This is the table that holds the command names, implementing functions, and
// brief description.
//
//*****************************************************************************
tCmdLineEntry g_sCmdTable[] =
{
    { "help",   Cmd_help,      " : Display list of commands" },
    { "h",      Cmd_help,   "    : alias for help" },
    { "?",      Cmd_help,   "    : alias for help" },
    { "ls",     Cmd_ls,      "   : Display list of files" },
    { "chdir",  Cmd_cd,         ": Change directory" },
    { "cd",     Cmd_cd,      "   : alias for chdir" },
    { "pwd",    Cmd_pwd,      "  : Show current working directory" },
    { "cat",    Cmd_cat,      "  : Show contents of a text file" },
    { 0, 0, 0 }
};

//*****************************************************************************
//
// This is the callback from the MSC driver.
//
// \param ulInstance is the driver instance which is needed when communicating
// with the driver.
// \param ulEvent is one of the events defined by the driver.
// \param pvData is a pointer to data passed into the initial call to register
// the callback.
//
// This function handles callback events from the MSC driver.  The only events
// currently handled are the MSC_EVENT_OPEN and MSC_EVENT_CLOSE.  This allows
// the main routine to know when an MSC device has been detected and
// enumerated and when an MSC device has been removed from the system.
//
// \return Returns \e true on success or \e false on failure.
//
//*****************************************************************************
void
MSCCallback(tUSBHMSCInstance *psMSCInstance, unsigned long ulEvent, void *pvData)
{
    //
    // Determine the event.
    //
    switch(ulEvent)
    {
        //
        // Called when the device driver has successfully enumerated an MSC
        // device.
        //
    case MSC_EVENT_OPEN:
    {
        //
        // Proceed to the enumeration state.
        //
        g_eState = STATE_DEVICE_ENUM;
        break;
    }

    //
    // Called when the device driver has been unloaded due to error or
    // the device is no longer present.
    //
    case MSC_EVENT_CLOSE:
    {
        //
        // Go back to the "no device" state and wait for a new connection.
        //
        g_eState = STATE_NO_DEVICE;

        break;
    }

    default:
    {
        break;
    }
    }
}

//*****************************************************************************
//
// This is the generic callback from host stack.
//
// \param pvData is actually a pointer to a tEventInfo structure.
//
// This function will be called to inform the application when a USB event has
// occurred that is outside those related to the mass storage device.  At this
// point this is used to detect unsupported devices being inserted and removed.
// It is also used to inform the application when a power fault has occurred.
// This function is required when the g_USBGenericEventDriver is included in
// the host controller driver array that is passed in to the
// USBHCDRegisterDrivers() function.
//
// \return None.
//
//*****************************************************************************
void
USBHCDEvents(void *pvData)
{
    tEventInfo *pEventInfo;

    //
    // Cast this pointer to its actual type.
    //
    pEventInfo = (tEventInfo *)pvData;

    switch(pEventInfo->ui32Event)
    {
        //
        // New keyboard detected.
        //
    case USB_EVENT_UNKNOWN_CONNECTED:
    {
        //
        // An unknown device was detected.
        //
        g_eState = STATE_UNKNOWN_DEVICE;

        break;
    }

    //
    // Keyboard has been unplugged.
    //
    case USB_EVENT_DISCONNECTED:
    {
        //
        // Unknown device has been removed.
        //
        g_eState = STATE_NO_DEVICE;

        break;
    }

    case USB_EVENT_POWER_FAULT:
    {
        //
        // No power means no device is present.
        //
        g_eState = STATE_POWER_FAULT;

        break;
    }

    default:
    {
        break;
    }
    }
}

//*****************************************************************************
//
// This function reads a line of text from the UART console.  The USB host main
// function is called throughout this process to keep USB alive and well.
//
//*****************************************************************************
void
ReadLine(void)
{
    unsigned long ulIdx, ulPrompt;
    unsigned char ucChar;
    tState eStateCopy;

    //
    // Start reading at the beginning of the command buffer and print a prompt.
    //
    g_cCmdBuf[0] = '\0';
    ulIdx = 0;
    ulPrompt = 1;

    //
    // Loop forever.  This loop will be explicitly broken out of when the line
    // has been fully read.
    //
    while(1)
    {

        //
        // See if a mass storage device has been enumerated.
        //
        if(g_eState == STATE_DEVICE_ENUM)
        {
            //
            // Take it easy on the Mass storage device if it is slow to
            // start up after connecting.
            //
            if(USBHMSCDriveReady(g_psMSCInstance) != 0)
            {
                //
                // Wait about 100ms before attempting to check if the
                // device is ready again.
                //
                SysCtlDelay(SysCtlClockGet(SYSTEM_CLOCK_SPEED)/30);

                break;
            }

            //
            // Reset the working directory to the root.
            //
            g_cCwdBuf[0] = '/';
            g_cCwdBuf[1] = '\0';

            //
            // Attempt to open the directory.  Some drives take longer to
            // start up than others, and this may fail (even though the USB
            // device has enumerated) if it is still initializing.
            //
            f_mount(0, &g_sFatFs);
            if(f_opendir(&g_sDirObject, g_cCwdBuf) == FR_OK)
            {
                //
                // The drive is fully ready, so move to that state.
                //
                g_eState = STATE_DEVICE_READY;
            }
        }

        //
        // See if the state has changed.  We make a copy of g_eUIState to
        // prevent a compiler warning about undefined order of volatile
        // accesses.
        //
        eStateCopy = g_eUIState;
        if(g_eState != eStateCopy)
        {
            //
            // Determine the new state.
            //
            switch(g_eState)
            {
                //
                // A previously connected device has been disconnected.
                //
            case STATE_NO_DEVICE:
            {
                if(g_eUIState == STATE_UNKNOWN_DEVICE)
                {
                    UARTprintf("\nUnknown device disconnected.\n");
                }
                else
                {
                    UARTprintf("\nMass storage device disconnected.\n");
                }
                ulPrompt = 1;
                break;
            }

            //
            // A mass storage device is being enumerated.
            //
            case STATE_DEVICE_ENUM:
            {
                break;
            }

            //
            // A mass storage device has been enumerated and initialized.
            //
            case STATE_DEVICE_READY:
            {
                UARTprintf("\nMass storage device connected.\n");
                ulPrompt = 1;
                break;
            }

            //
            // An unknown device has been connected.
            //
            case STATE_UNKNOWN_DEVICE:
            {
                UARTprintf("\nUnknown device connected.\n");
                ulPrompt = 1;
                break;
            }

            //
            // A power fault has occurred.
            //
            case STATE_POWER_FAULT:
            {
                UARTprintf("\nPower fault.\n");
                ulPrompt = 1;
                break;
            }
            }

            //
            // Save the current state.
            //
            g_eUIState = g_eState;
        }

        //
        // Print a prompt if necessary.
        //
        if(ulPrompt)
        {
            //
            // Print the prompt based on the current state.
            //
            if(g_eState == STATE_DEVICE_READY)
            {
                UARTprintf("%s> %s", g_cCwdBuf, g_cCmdBuf);
            }
            else if(g_eState == STATE_UNKNOWN_DEVICE)
            {
                UARTprintf("UNKNOWN> %s", g_cCmdBuf);
            }
            else
            {
                UARTprintf("NODEV> %s", g_cCmdBuf);
            }

            //
            // The prompt no longer needs to be printed.
            //
            ulPrompt = 0;
        }

        //
        // Loop while there are characters that have been received from the
        // UART.
        //
        while(UARTCharsAvail(UART0_BASE))
        {
            //
            // Read the next character from the UART.
            //
            ucChar = UARTgetc();

            //
            // See if this character is a backspace and there is at least one
            // character in the input line.
            //
            if((ucChar == '\b') && (ulIdx != 0))
            {
                //
                // Erase the lsat character from the input line.
                //
                UARTprintf("\b \b");
                ulIdx--;
                g_cCmdBuf[ulIdx] = '\0';
            }

            //
            // See if this character is a newline.
            //
            else if((ucChar == '\r') || (ucChar == '\n'))
            {
                //
                // Return to the caller.
                //
                UARTprintf("\n");
                return;
            }

            //
            // See if this character is an escape or Ctrl-U.
            //
            else if((ucChar == 0x1b) || (ucChar == 0x15))
            {
                //
                // Erase all characters in the input buffer.
                //
                while(ulIdx)
                {
                    UARTprintf("\b \b");
                    ulIdx--;
                }
                g_cCmdBuf[0] = '\0';
            }

            //
            // See if this is a printable ASCII character.
            //
            else if((ucChar >= ' ') && (ucChar <= '~') &&
                    (ulIdx < (sizeof(g_cCmdBuf) - 1)))
            {
                //
                // Add this character to the input buffer.
                //
                g_cCmdBuf[ulIdx++] = ucChar;
                g_cCmdBuf[ulIdx] = '\0';
                UARTprintf("%c", (unsigned long)ucChar);
            }
        }

        //
        // Tell the OTG state machine how much time has passed in
        // milliseconds since the last call.
        //
        USBHCDMain();
//        USBOTGMain(GetTickms());
    }
}


void SysCtrlInit(void)
{

    EALLOW;
    //Disable Watchdog
    SysCtrlRegs.WDCR = 0x68;

    //Setup Clock
    //20MHz ->PLL->80MHz->C28
    //      ->PLL2->120MHz->USB
    SysCtrlRegs.CLKCTL.bit.XCLKINOFF = 1;
    SysCtrlRegs.CLKCTL.bit.XTALOSCOFF = 0;
    SysCtrlRegs.CLKCTL.bit.OSCCLKSRC2SEL = 0;
    SysCtrlRegs.CLKCTL.bit.OSCCLKSRCSEL = 1;

    while(SysCtrlRegs.PLLSTS.bit.MCLKSTS);
    SysCtrlRegs.PLLSTS.bit.DIVSEL = 0;
    SysCtrlRegs.PLLSTS.bit.MCLKOFF = 1;
    SysCtrlRegs.PLLCR.bit.DIV = 4;
    while(!SysCtrlRegs.PLLSTS.bit.PLLLOCKS);
    SysCtrlRegs.PLLSTS.bit.MCLKOFF = 1;
    SysCtrlRegs.PLLSTS.bit.DIVSEL = 3;

    SysCtrlRegs.PLL2CTL.bit.PLL2CLKSRCSEL = 2;
    SysCtrlRegs.PLL2CTL.bit.PLL2EN = 1;
    SysCtrlRegs.PLL2MULT.bit.PLL2MULT = 6;
    while(!SysCtrlRegs.PLL2STS.bit.PLL2LOCKS);

    //Enable USB Clock
    SysCtrlRegs.PCLKCR3.bit.USB0ENCLK = 1;
    //Enable PHY
    GpioCtrlRegs.GPACTRL2.bit.USB0IOEN = 1;

    EDIS;
}

//*****************************************************************************
//
// This is the main loop that runs the application.
//
//*****************************************************************************
int
main(void)
{
    int iStatus;

    //
    // Initially wait for device connection.
    //
    g_eState = STATE_NO_DEVICE;
    g_eUIState = STATE_NO_DEVICE;

    //
    // Set the clocking to run from the PLL at 60MHz.
    //
    SysCtrlInit();

    //
    // Initialize PIE and vector table
    InitPieCtrl();
    InitPieVectTable();
    
    //
    // Enable Host mode
    //
    EALLOW;
    HWREG(USBMODESEL) = USBMODESEL_HOST;
    HWREG(USB0_BASE + USB_O_GPCS) = USBGPCS_HOST;
    EDIS;

    //
    // Enable the UART and setup GPIOs
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    EALLOW;
    GpioCtrlRegs.GPAMUX2.bit.GPIO28 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO28 = 0;
    GpioCtrlRegs.GPAQSEL2.bit.GPIO28 = 3;
    GpioCtrlRegs.GPADIR.bit.GPIO28 = 0;

    GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 1;
    GpioCtrlRegs.GPAPUD.bit.GPIO29 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO29 = 1;
    EDIS;

    UARTStdioInit(0);

    //
    // Enable Clocking to the USB controller.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_USB0);

    //
    // Register interrupt handler and enable interrupts
    //
    USBGPIOEnable();
    USBIntRegister(USB0_BASE, f28x_USB0HostIntHandler);
    IntMasterEnable();

    UARTprintf("\n\nUSB Mass Storage Host program\n");
    UARTprintf("Type \'help\' for help.\n\n");

    //
    // Initialize the USB stack mode and pass in a mode callback.
    //
    USBStackModeSet(0, eUSBModeForceHost, ModeCallback);

    //
    // Register the host class drivers.
    //
    USBHCDRegisterDrivers(0, g_ppHostClassDrivers, NUM_CLASS_DRIVERS);

    //
    // Open an instance of the mass storage class driver.
    //
    g_psMSCInstance = USBHMSCDriveOpen(0, (tUSBHMSCCallback)MSCCallback);

    //
    // Initialize the power configuration. This sets the power enable signal
    // to be active high and does not enable the power fault.
    //
    USBHCDPowerConfigInit(0, USBHCD_VBUS_AUTO_HIGH | USBHCD_VBUS_FILTER);

    //
    // Initialize the USB controller for OTG operation with a 2ms polling
    // rate.
    //
	USBHCDInit(0,g_pHCDPool, HCD_MEMORY_SIZE);

    //
    // Initialize the file system.
    //
    f_mount(0, &g_sFatFs);

    //
    // Enter an infinite loop for reading and processing commands from the
    // user.
    //
    while(1)
    {
        //
        // Get a line of text from the user.
        //
        ReadLine();
        if(g_cCmdBuf[0] == '\0')
        {
            continue;
        }

        //
        // Pass the line from the user to the command processor.
        // It will be parsed and valid commands executed.
        //
        iStatus = CmdLineProcess(g_cCmdBuf);

        //
        // Handle the case of bad command.
        //
        if(iStatus == CMDLINE_BAD_CMD)
        {
            UARTprintf("Bad command!\n");
        }

        //
        // Handle the case of too many arguments.
        //
        else if(iStatus == CMDLINE_TOO_MANY_ARGS)
        {
            UARTprintf("Too many arguments for command processor!\n");
        }

        //
        // Otherwise the command was executed.  Print the error
        // code if one was returned.
        //
        else if(iStatus != 0)
        {
            UARTprintf("Command returned error code %s\n",
                       StringFromFresult((FRESULT)iStatus));
        }
    }
}
