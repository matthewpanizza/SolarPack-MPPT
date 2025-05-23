/*-----------------------------------------------------------------------*/
/* MMC/SDC (in SPI mode) control module  (C)ChaN, 2007                   */
/*-----------------------------------------------------------------------*/
/* Only rcvr_spi(), xmit_spi(), disk_timerproc() and some macros         */
/* are platform dependent.                                               */
/*-----------------------------------------------------------------------*/

/*
 * This file was modified from a sample available from the FatFs
 * web site. It was modified to work with a Stellaris DK-LM3S9B96 board.
 */

#include "DSP28x_Project.h"
//#include "F2806x_SPI_defines.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "third_party/fatfs/src/diskio.h"

/*
 * The following header defines the hardware connections used to connect
 * the SDCard.  This can be found under the relevant board directory.
 */


/* Definitions for MMC/SDC command */
#define CMD0    (0x40+0)    /* GO_IDLE_STATE */
#define CMD1    (0x40+1)    /* SEND_OP_COND */
#define CMD8    (0x40+8)    /* SEND_IF_COND */
#define CMD9    (0x40+9)    /* SEND_CSD */
#define CMD10    (0x40+10)    /* SEND_CID */
#define CMD12    (0x40+12)    /* STOP_TRANSMISSION */
#define CMD16    (0x40+16)    /* SET_BLOCKLEN */
#define CMD17    (0x40+17)    /* READ_SINGLE_BLOCK */
#define CMD18    (0x40+18)    /* READ_MULTIPLE_BLOCK */
#define CMD23    (0x40+23)    /* SET_BLOCK_COUNT */
#define CMD24    (0x40+24)    /* WRITE_BLOCK */
#define CMD25    (0x40+25)    /* WRITE_MULTIPLE_BLOCK */
#define CMD41    (0x40+41)    /* SEND_OP_COND (ACMD) */
#define CMD55    (0x40+55)    /* APP_CMD */
#define CMD58    (0x40+58)    /* READ_OCR */

// asserts the CS pin to the card
static
void SELECT (void)
{
	
	//TODO: Pull CS low
//    //
//    // Make sure that the serial flash which shares this SSI port is
//    // not selected.
//    //
//    GPIOPinWrite(SFLASH_CS_BASE, SFLASH_CS_PIN, SFLASH_CS_PIN);
//
//    //
//    // Select the SD card.
//    //
//    GPIOPinWrite(SDCARD_CS_BASE, SDCARD_CS_PIN, 0);

	GpioDataRegs.GPBDAT.bit.GPIO57 = 0;
}

// de-asserts the CS pin to the card.
static
void DESELECT (void)
{
	//TODO: Pull CS High
//    GPIOPinWrite(SDCARD_CS_BASE, SDCARD_CS_PIN, SDCARD_CS_PIN);
	GpioDataRegs.GPBDAT.bit.GPIO57 = 1;
}

/*--------------------------------------------------------------------------

   Module Private Functions

---------------------------------------------------------------------------*/

static volatile
DSTATUS Stat = STA_NOINIT;    /* Disk status */

static volatile
BYTE Timer1, Timer2;    /* 100Hz decrement timer */

static
BYTE CardType;            /* b0:MMC, b1:SDC, b2:Block addressing */

static
BYTE PowerFlag = 0;     /* indicates if "power" is on */

/*-----------------------------------------------------------------------*/
/* Transmit a byte to MMC via SPI  (Platform dependent)                  */
/*-----------------------------------------------------------------------*/

static
void xmit_spi (BYTE dat)
{
//    DWORD rcvdat;
//
//    SPIDataPut(&SpiaRegs, dat); /* Write the data to the tx fifo */
//
//    SPIDataGet(&SpiaRegs, (unsigned int *) &rcvdat); /* flush data read during the write */

	spi_xmit(&SpiaRegs, dat << 8);
}


/*-----------------------------------------------------------------------*/
/* Receive a byte from MMC via SPI  (Platform dependent)                 */
/*-----------------------------------------------------------------------*/

static
BYTE rcvr_spi (void)
{
//    DWORD rcvdat;
//
//    SPIDataPut(&SpiaRegs, 0xFF); /* write dummy data */
//
//    SPIDataGet(&SpiaRegs, (unsigned int *) &rcvdat); /* read data frm rx fifo */
//
//    return (BYTE)rcvdat;
	return spi_xmit(&SpiaRegs, 0xFF << 8);
}


static
void rcvr_spi_m (BYTE *dst)
{
    *dst = rcvr_spi();
}

/*-----------------------------------------------------------------------*/
/* Wait for card ready                                                   */
/*-----------------------------------------------------------------------*/

static
BYTE wait_ready (void)
{
    BYTE res;


    Timer2 = 50;    /* Wait for ready in timeout of 500ms */
    rcvr_spi();
    do
        res = rcvr_spi();
    while ((res != 0xFF) && Timer2);

    return res;
}

/*-----------------------------------------------------------------------*/
/* Send 80 or so clock transitions with CS and DI held high. This is     */
/* required after card power up to get it into SPI mode                  */
/*-----------------------------------------------------------------------*/
static
void send_initial_clock_train (void)
{
    unsigned int i;
    DWORD dat;

    /* Ensure CS is held high. */
    DESELECT();

    /* Switch the SSI TX line to a GPIO and drive it high too. */
//    GPIOPinTypeGPIOOutput(SDC_GPIO_PORT_BASE, SDC_SSI_TX);
//    GPIOPinWrite(SDC_GPIO_PORT_BASE, SDC_SSI_TX, SDC_SSI_TX);

    /* Send 10 bytes over the SSI. This causes the clock to wiggle the */
    /* required number of times. */
    for(i = 0 ; i < 10 ; i++)
    {
        /* Write DUMMY data. SSIDataPut() waits until there is room in the */
        /* FIFO. */
		rcvr_spi();

    }

    /* Revert to hardware control of the SSI TX line. */
//    GPIOPinTypeSSI(SDC_GPIO_PORT_BASE, SDC_SSI_TX);
}

/*-----------------------------------------------------------------------*/
/* Power Control  (Platform dependent)                                   */
/*-----------------------------------------------------------------------*/
/* When the target system does not support socket power control, there   */
/* is nothing to do in these functions and chk_power always returns 1.   */

static
void power_on (void)
{
	
	unsigned int test;
    /*
     * This doesn't really turn the power on, but initializes the
     * SSI port and pins needed to talk to the card.
     */
     
     EALLOW;

    /* Enable the peripherals used to drive the SDC on SPI */

	SysCtrlRegs.PCLKCR0.bit.SPIAENCLK = 1;


    /*
     * Configure the appropriate pins to be SSI instead of GPIO. The CS
     * signal is directly driven to ensure that we can hold it low through a
     * complete transaction with the SD card.
     */
     //TODO: Board Specific Pin Configuration
     //GPIO54 - MOSI - PER1
     //GPIO55 - MISO - PER1
     //GPIO56 - CLK - PER1
     //GPIO57 - CS - GPIO
     GpioCtrlRegs.GPBMUX2.bit.GPIO54 = 1;
     GpioCtrlRegs.GPBDIR.bit.GPIO54 = 1;
     //GpioCtrlRegs.GPBQSEL2.bit.GPIO54 = 3;
     
     GpioCtrlRegs.GPBMUX2.bit.GPIO55 = 1;
     GpioCtrlRegs.GPBDIR.bit.GPIO55 = 0;
     GpioCtrlRegs.GPBQSEL2.bit.GPIO55 = 3;     
     
     GpioCtrlRegs.GPBMUX2.bit.GPIO56 = 1;
     GpioCtrlRegs.GPBDIR.bit.GPIO56 = 1;
     //GpioCtrlRegs.GPBQSEL2.bit.GPIO56 = 3;     
     
     GpioCtrlRegs.GPBMUX2.bit.GPIO57 = 0;
     GpioCtrlRegs.GPBDIR.bit.GPIO57 = 1;
     //GpioCtrlRegs.GPBQSEL2.bit.GPIO57 = 3;    
     //CS High
     GpioDataRegs.GPBDAT.bit.GPIO57 = 1;
     
    
//     GpioDataRegs.GPBDAT.bit.GPIO54 = 1;
//     GpioDataRegs.GPBDAT.bit.GPIO55 = 1;
//     GpioDataRegs.GPBDAT.bit.GPIO56 = 1;
//     GpioDataRegs.GPBDAT.bit.GPIO57 = 1;
//     GpioDataRegs.GPBDAT.bit.GPIO54 = 0;
//     GpioDataRegs.GPBDAT.bit.GPIO55 = 0;
//     GpioDataRegs.GPBDAT.bit.GPIO56 = 0;
//     GpioDataRegs.GPBDAT.bit.GPIO57 = 0;
       
    /* Deassert the SSI0 chip selects for both the SD card and serial flash */
//    GPIOPinWrite(SDCARD_CS_BASE, SDCARD_CS_PIN, SDCARD_CS_PIN);
//    GPIOPinWrite(SFLASH_CS_BASE, SFLASH_CS_PIN, SFLASH_CS_PIN);

    /* Configure the SSI0 port */
    SysCtrlRegs.LOSPCP.all = 0;
    SpiaRegs.SPIBRR = 0;
    
    //InitSpi(&SpiaRegs, SPI_CR_SW_RESET | SPI_CR_CHAR_8 | SPI_CR_CLKPOL_RIS, SPI_CTL_MASTER | SPI_CTL_TALK, 0);
    //InitSpi(&SpiaRegs, SPI_CR_SW_RESET | SPI_CR_CHAR_8 | SPI_CR_CLKPOL_FAL, SPI_CTL_MASTER | SPI_CTL_TALK, 0);
    InitSpi(&SpiaRegs,  SPI_CR_SW_RESET | SPI_CR_CHAR_8 | SPI_CR_CLKPOL_RIS, SPI_CTL_MASTER | SPI_CTL_TALK | SPI_CTL_CLKPHS_90, 0x7F);
    //InitSpi(&SpiaRegs, SPI_CR_SW_RESET | SPI_CR_CHAR_8 | SPI_CR_CLKPOL_FAL, SPI_CTL_MASTER | SPI_CTL_TALK | SPI_CTL_CLKPHS_90, 0);
    //TODO: Set bit rate 
    //No divide on low speed clock


    
//    SSIConfigSetExpClk(SDC_SSI_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0,
//                       SSI_MODE_MASTER, 400000, 8);
//    SSIEnable(SDC_SSI_BASE);

    /* Set DI and CS high and apply more than 74 pulses to SCLK for the card */
    /* to be able to accept a native command. */
    send_initial_clock_train();

    PowerFlag = 1;
}

// set the SSI speed to the max setting
static
void set_max_speed(void)
{
    unsigned long i;

    /* Disable the SSI */
//    SSIDisable(SDC_SSI_BASE);
	
    /* Set the maximum speed as half the system clock, with a max of 12.5 MHz. */
//    i = SysCtlClockGet() / 2;
    if(i > 12500000)
    {
        i = 12500000;
    }
    
    SpiaRegs.SPIBRR =0x0030;

    /* Configure the SSI0 port */
//    SSIConfigSetExpClk(SDC_SSI_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0,
//                       SSI_MODE_MASTER, i, 8);

    /* Enable the SSI */
//    SSIEnable(SDC_SSI_BASE);
}

static
void power_off (void)
{
    PowerFlag = 0;
}

static
int chk_power(void)        /* Socket power state: 0=off, 1=on */
{
    return PowerFlag;
}



/*-----------------------------------------------------------------------*/
/* Receive a data packet from MMC                                        */
/*-----------------------------------------------------------------------*/

static
BOOL rcvr_datablock (
    BYTE *buff,            /* Data buffer to store received data */
    UINT btr            /* Byte count (must be even number) */
)
{
    BYTE token;


    Timer1 = 10;
    do {                            /* Wait for data packet in timeout of 100ms */
        token = rcvr_spi();
    } while ((token == 0xFF) && Timer1);
    if(token != 0xFE) return FALSE;    /* If not valid data token, retutn with error */

    do {                            /* Receive the data block into buffer */
        rcvr_spi_m(buff++);
        rcvr_spi_m(buff++);
    } while (btr -= 2);
    rcvr_spi();                        /* Discard CRC */
    rcvr_spi();

    return TRUE;                    /* Return with success */
}



/*-----------------------------------------------------------------------*/
/* Send a data packet to MMC                                             */
/*-----------------------------------------------------------------------*/

#if _READONLY == 0
static
BOOL xmit_datablock (
    const BYTE *buff,    /* 512 byte data block to be transmitted */
    BYTE token            /* Data/Stop token */
)
{
    BYTE resp, wc;


    if (wait_ready() != 0xFF) return FALSE;

    xmit_spi(token);                    /* Xmit data token */
    if (token != 0xFD) {    /* Is data token */
        wc = 0;
        do {                            /* Xmit the 512 byte data block to MMC */
            xmit_spi(*buff++);
            xmit_spi(*buff++);
        } while (--wc);
        xmit_spi(0xFF);                    /* CRC (Dummy) */
        xmit_spi(0xFF);
        resp = rcvr_spi();                /* Reveive data response */
        if ((resp & 0x1F) != 0x05)        /* If not accepted, return with error */
            return FALSE;
    }

    return TRUE;
}
#endif /* _READONLY */



/*-----------------------------------------------------------------------*/
/* Send a command packet to MMC                                          */
/*-----------------------------------------------------------------------*/

static
BYTE send_cmd (
    BYTE cmd,        /* Command byte */
    DWORD arg        /* Argument */
)
{
    BYTE n, res;


    if (wait_ready() != 0xFF) return 0xFF;

    /* Send command packet */
    xmit_spi(cmd);                        /* Command */
    xmit_spi((BYTE)(arg >> 24));        /* Argument[31..24] */
    xmit_spi((BYTE)(arg >> 16));        /* Argument[23..16] */
    xmit_spi((BYTE)(arg >> 8));            /* Argument[15..8] */
    xmit_spi((BYTE)arg);                /* Argument[7..0] */
    n = 0;
    if (cmd == CMD0) n = 0x95;            /* CRC for CMD0(0) */
    if (cmd == CMD8) n = 0x87;            /* CRC for CMD8(0x1AA) */
    xmit_spi(n);

    /* Receive command response */
    if (cmd == CMD12) rcvr_spi();        /* Skip a stuff byte when stop reading */
    n = 10;                                /* Wait for a valid response in timeout of 10 attempts */
    do
        res = rcvr_spi();
    while ((res & 0x80) && --n);

    return res;            /* Return with the response value */
}



/*--------------------------------------------------------------------------

   Public Functions

---------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
    BYTE drv        /* Physical drive nmuber (0) */
)
{
    BYTE n, ty, ocr[4];


    if (drv) return STA_NOINIT;            /* Supports only single drive */
    if (Stat & STA_NODISK) return Stat;    /* No card in the socket */

    power_on();                            /* Force socket power on */
    send_initial_clock_train();

    SELECT();                /* CS = L */
    ty = 0;
    if (send_cmd(CMD0, 0) == 1) {            /* Enter Idle state */
        Timer1 = 100;                        /* Initialization timeout of 1000 msec */
        if (send_cmd(CMD8, 0x1AA) == 1) {    /* SDC Ver2+ */
            for (n = 0; n < 4; n++) ocr[n] = rcvr_spi();
            if (ocr[2] == 0x01 && ocr[3] == 0xAA) {    /* The card can work at vdd range of 2.7-3.6V */
                do {
                    if (send_cmd(CMD55, 0) <= 1 && send_cmd(CMD41, 1UL << 30) == 0)    break;    /* ACMD41 with HCS bit */
                } while (Timer1);
                if (Timer1 && send_cmd(CMD58, 0) == 0) {    /* Check CCS bit */
                    for (n = 0; n < 4; n++) ocr[n] = rcvr_spi();
                    ty = (ocr[0] & 0x40) ? 6 : 2;
                }
            }
        } else {                            /* SDC Ver1 or MMC */
            ty = (send_cmd(CMD55, 0) <= 1 && send_cmd(CMD41, 0) <= 1) ? 2 : 1;    /* SDC : MMC */
            do {
                if (ty == 2) {
                    if (send_cmd(CMD55, 0) <= 1 && send_cmd(CMD41, 0) == 0) break;    /* ACMD41 */
                } else {
                    if (send_cmd(CMD1, 0) == 0) break;                                /* CMD1 */
                }
            } while (Timer1);
            if (!Timer1 || send_cmd(CMD16, 512) != 0)    /* Select R/W block length */
                ty = 0;
        }
    }
    CardType = ty;
    DESELECT();            /* CS = H */
    rcvr_spi();            /* Idle (Release DO) */

    if (ty) {            /* Initialization succeded */
        Stat &= ~STA_NOINIT;        /* Clear STA_NOINIT */
        set_max_speed();
    } else {            /* Initialization failed */
        power_off();
    }

    return Stat;
}



/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
    BYTE drv        /* Physical drive nmuber (0) */
)
{
    if (drv) return STA_NOINIT;        /* Supports only single drive */
    return Stat;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
    BYTE drv,            /* Physical drive nmuber (0) */
    BYTE *buff,            /* Pointer to the data buffer to store read data */
    DWORD sector,        /* Start sector number (LBA) */
    BYTE count            /* Sector count (1..255) */
)
{
    if (drv || !count) return RES_PARERR;
    if (Stat & STA_NOINIT) return RES_NOTRDY;

    if (!(CardType & 4)) sector *= 512;    /* Convert to byte address if needed */

    SELECT();            /* CS = L */

    if (count == 1) {    /* Single block read */
        if ((send_cmd(CMD17, sector) == 0)    /* READ_SINGLE_BLOCK */
            && rcvr_datablock(buff, 512))
            count = 0;
    }
    else {                /* Multiple block read */
        if (send_cmd(CMD18, sector) == 0) {    /* READ_MULTIPLE_BLOCK */
            do {
                if (!rcvr_datablock(buff, 512)) break;
                buff += 512;
            } while (--count);
            send_cmd(CMD12, 0);                /* STOP_TRANSMISSION */
        }
    }

    DESELECT();            /* CS = H */
    rcvr_spi();            /* Idle (Release DO) */

    return count ? RES_ERROR : RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _READONLY == 0
DRESULT disk_write (
    BYTE drv,            /* Physical drive nmuber (0) */
    const BYTE *buff,    /* Pointer to the data to be written */
    DWORD sector,        /* Start sector number (LBA) */
    BYTE count            /* Sector count (1..255) */
)
{
    if (drv || !count) return RES_PARERR;
    if (Stat & STA_NOINIT) return RES_NOTRDY;
    if (Stat & STA_PROTECT) return RES_WRPRT;

    if (!(CardType & 4)) sector *= 512;    /* Convert to byte address if needed */

    SELECT();            /* CS = L */

    if (count == 1) {    /* Single block write */
        if ((send_cmd(CMD24, sector) == 0)    /* WRITE_BLOCK */
            && xmit_datablock(buff, 0xFE))
            count = 0;
    }
    else {                /* Multiple block write */
        if (CardType & 2) {
            send_cmd(CMD55, 0); send_cmd(CMD23, count);    /* ACMD23 */
        }
        if (send_cmd(CMD25, sector) == 0) {    /* WRITE_MULTIPLE_BLOCK */
            do {
                if (!xmit_datablock(buff, 0xFC)) break;
                buff += 512;
            } while (--count);
            if (!xmit_datablock(0, 0xFD))    /* STOP_TRAN token */
                count = 1;
        }
    }

    DESELECT();            /* CS = H */
    rcvr_spi();            /* Idle (Release DO) */

    return count ? RES_ERROR : RES_OK;
}
#endif /* _READONLY */



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
    BYTE drv,        /* Physical drive nmuber (0) */
    BYTE ctrl,        /* Control code */
    void *buff        /* Buffer to send/receive control data */
)
{
    DRESULT res;
    BYTE n, csd[16], *ptr = buff;
    WORD csize;


    if (drv) return RES_PARERR;

    res = RES_ERROR;

    if (ctrl == CTRL_POWER) {
        switch (*ptr) {
        case 0:        /* Sub control code == 0 (POWER_OFF) */
            if (chk_power())
                power_off();        /* Power off */
            res = RES_OK;
            break;
        case 1:        /* Sub control code == 1 (POWER_ON) */
            power_on();                /* Power on */
            res = RES_OK;
            break;
        case 2:        /* Sub control code == 2 (POWER_GET) */
            *(ptr+1) = (BYTE)chk_power();
            res = RES_OK;
            break;
        default :
            res = RES_PARERR;
        }
    }
    else {
        if (Stat & STA_NOINIT) return RES_NOTRDY;

        SELECT();        /* CS = L */

        switch (ctrl) {
        case GET_SECTOR_COUNT :    /* Get number of sectors on the disk (DWORD) */
            if ((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd, 16)) {
                if ((csd[0] >> 6) == 1) {    /* SDC ver 2.00 */
                    csize = csd[9] + ((WORD)csd[8] << 8) + 1;
                    *(DWORD*)buff = (DWORD)csize << 10;
                } else {                    /* MMC or SDC ver 1.XX */
                    n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
                    csize = (csd[8] >> 6) + ((WORD)csd[7] << 2) + ((WORD)(csd[6] & 3) << 10) + 1;
                    *(DWORD*)buff = (DWORD)csize << (n - 9);
                }
                res = RES_OK;
            }
            break;

        case GET_SECTOR_SIZE :    /* Get sectors on the disk (WORD) */
            *(WORD*)buff = 512;
            res = RES_OK;
            break;

        case CTRL_SYNC :    /* Make sure that data has been written */
            if (wait_ready() == 0xFF)
                res = RES_OK;
            break;

        case MMC_GET_CSD :    /* Receive CSD as a data block (16 bytes) */
            if (send_cmd(CMD9, 0) == 0        /* READ_CSD */
                && rcvr_datablock(ptr, 16))
                res = RES_OK;
            break;

        case MMC_GET_CID :    /* Receive CID as a data block (16 bytes) */
            if (send_cmd(CMD10, 0) == 0        /* READ_CID */
                && rcvr_datablock(ptr, 16))
                res = RES_OK;
            break;

        case MMC_GET_OCR :    /* Receive OCR as an R3 resp (4 bytes) */
            if (send_cmd(CMD58, 0) == 0) {    /* READ_OCR */
                for (n = 0; n < 4; n++)
                    *ptr++ = rcvr_spi();
                res = RES_OK;
            }

//        case MMC_GET_TYPE :    /* Get card type flags (1 byte) */
//            *ptr = CardType;
//            res = RES_OK;
//            break;

        default:
            res = RES_PARERR;
        }

        DESELECT();            /* CS = H */
        rcvr_spi();            /* Idle (Release DO) */
    }

    return res;
}



/*-----------------------------------------------------------------------*/
/* Device Timer Interrupt Procedure  (Platform dependent)                */
/*-----------------------------------------------------------------------*/
/* This function must be called in period of 10ms                        */

void disk_timerproc (void)
{
//    BYTE n, s;
    BYTE n;


    n = Timer1;                        /* 100Hz decrement timer */
    if (n) Timer1 = --n;
    n = Timer2;
    if (n) Timer2 = --n;

}

/*---------------------------------------------------------*/
/* User Provided Timer Function for FatFs module           */
/*---------------------------------------------------------*/
/* This is a real time clock service to be called from     */
/* FatFs module. Any valid time must be returned even if   */
/* the system does not support a real time clock.          */

DWORD get_fattime (void)
{

    return    ((2008UL-1980) << 25)    // Year = 2008
            | (2UL << 21)            // Month = February
            | (26UL << 16)            // Day = 26
            | (14U << 11)            // Hour = 14
            | (0U << 5)            // Min = 0
            | (0U >> 1)                // Sec = 0
            ;

}


