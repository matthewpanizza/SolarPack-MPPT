//###########################################################################
//
// FILE:   hw_uart.h
//
// TITLE:  Definitions for the C28x SCI registers.
//
//###########################################################################
// $TI Release: F2806x C/C++ Header Files and Peripheral Examples V151 $
// $Release Date: February  2, 2016 $
// $Copyright: Copyright (C) 2011-2016 Texas Instruments Incorporated -
//             http://www.ti.com/ ALL RIGHTS RESERVED $
//###########################################################################

#ifndef __HW_UART_H__
#define __HW_UART_H__

//*****************************************************************************
//
// The following are defines for the UART register offsets.
//
//*****************************************************************************
#define UART_O_CR                 0x00000000  // UART Communications Control
#define UART_O_CTL1               0x00000001  // UART Control 1
#define UART_O_HBAUD              0x00000002  // UART Baud High
#define UART_O_LBAUD              0x00000003  // UART Baud Low
#define UART_O_CTL2               0x00000004  // UART Control 2
#define UART_O_RXST               0x00000005  // UART Receive Status
#define UART_O_RXEMU              0x00000006  // UART Receive Emulation Data
#define UART_O_RXBUF              0x00000007  // UART Receive Data Buffer
#define UART_O_TXBUF              0x00000009  // UART Transmit Data Buffer
#define UART_O_FFTX               0x0000000A  // UART Transmit FIFO
#define UART_O_FFRX               0x0000000B  // UART Receive FIFO
#define UART_O_FFCT               0x0000000C  // UART FIFO Control
#define UART_O_PRI                0x0000000F  // UART Priority Control

//*****************************************************************************
//
// The following are defines for the bit fields in the UART_O_CR register.
//
//*****************************************************************************
#define UART_CR_STOP              0x80        // UART Stop bits
#define UART_CR_PARITYEO          0x40        // UART Parity Even/Odd
#define UART_CR_PARITYEN          0x20        // UART Parity Enable
#define UART_CR_LB                0x10        // UART Loopback Enable
#define UART_CR_ADDRIDL           0x08        // UART Mulitprocessor Mode Ctrl
#define UART_CR_CHAR_M            0x07        // UART Character Length Mask

//*****************************************************************************
//
// The following are defines for the bit fields in the UART_O_CTL1 register.
//
//*****************************************************************************
#define UART_CTL1_RXERIE          0x40        // UART RX Error Interrupt Enable
#define UART_CTL1_SWRST           0x20        // UART Software Reset
#define UART_CTL1_TXWAKE          0x08        // UART Transmit wakeup select
#define UART_CTL1_SLEEP           0x04        // UART Sleep Mode
#define UART_CTL1_TXENA           0x02        // UART Transmitter Enable
#define UART_CTL1_RXENA           0x01        // UART Receiver Enable

//*****************************************************************************
//
// The following are defines for the bit fields in the UART_O_HBAUD register.
//
//*****************************************************************************
#define UART_HBAUD_BAUD           0xFF        // UART Baud Selection Bits 15-8

//*****************************************************************************
//
// The following are defines for the bit fields in the UART_O_LBAUD register.
//
//*****************************************************************************
#define UART_LBAUD_BAUD           0xFF        // UART Baud Selection Bits 7-0

//*****************************************************************************
//
// The following are defines for the bit fields in the UART_O_CTL2 register.
//
//*****************************************************************************
#define UART_CTL2_TXRDY           0x80        // UART Transmit Buffer Ready
#define UART_CTL2_TXEMPTY         0x40        // UART Transmit Buffer Empty
#define UART_CTL2_RXBKINTEN       0x02        // UART RX buf/break Int Enable
#define UART_CTL2_TXINTEN         0x01        // UART TXRDY Int Enable

//*****************************************************************************
//
// The following are defines for the bit fields in the UART_O_RXST register.
//
//*****************************************************************************
#define UART_RXST_RXERR           0x80        // UART RX Error
#define UART_RXST_RXRDY           0x40        // UART RX Ready
#define UART_RXST_BRKDT           0x20        // UART Break Detect
#define UART_RXST_FE              0x10        // UART Framing Error
#define UART_RXST_OE              0x08        // UART Overrun Error
#define UART_RXST_PE              0x40        // UART Parity Error
#define UART_RXST_RXWAKE          0x20        // UART RX Wakeup

//*****************************************************************************
//
// The following are defines for the bit fields in the UART_O_RXEMU register.
//
//*****************************************************************************
#define UART_RXEMU_ERXDT_M        0xFF        // UART RX Emulation Data Buffer

//*****************************************************************************
//
// The following are defines for the bit fields in the UART_O_RXBUF register.
//
//*****************************************************************************
#define UART_RXBUF_FFFE           0x8000      // UART FIFO Framing Error
#define UART_RXBUF_FFPE           0x4000      // UART FIFO Parity Error
#define UART_RXBUF_RXDT_M         0x00FF      // UART Receive Buffer

//*****************************************************************************
//
// The following are defines for the bit fields in the UART_O_TXBUF register.
//
//*****************************************************************************
#define UART_TXBUF_TXDT_M         0xFF        // UART Transmit Buffer

//*****************************************************************************
//
// The following are defines for the bit fields in the UART_O_FFTX register.
//
//*****************************************************************************
#define UART_FFTX_RST             0x8000      // UART Reset
#define UART_FFTX_FFENA           0x4000      // UART FIFO Enable
#define UART_FFTX_TXFFRST         0x2000      // UART TX FIFO Reset
#define UART_FFTX_TXFFST_M        0x1F00      // UART TX FIFO Status Mask
#define UART_FFTX_TXFFST_S        0x0008      // UART TX FIFO Status Shift
#define UART_FFTX_TXFFINT         0x0080      // UART TX FIFO Interrupt
#define UART_FFTX_TXFFINTCLR      0x0040      // UART TX FIFO Interrupt Clear
#define UART_FFTX_TXFFIENA        0x0020      // UART TX FIFO Interrupt Enable
#define UART_FFTX_TXFFIL_M        0x001F      // UART TX FIFO Interrupt Level

//*****************************************************************************
//
// The following are defines for the bit fields in the UART_O_FFRX register.
//
//*****************************************************************************
#define UART_FFRX_OVF             0x8000      // UART RX FIFO Overflow
#define UART_FFRX_OVFCLR          0x4000      // UART RX FIFO Overflow Clear
#define UART_FFRX_RXFFRST         0x2000      // UART RX FIFO Reset
#define UART_FFRX_RXFFST_M        0x1F00      // UART RX FIFO Status
#define UART_FFTX_RXFFST_S        0x0008      // UART RX FIFO Status Shift
#define UART_FFRX_RXFFINT         0x0080      // UART RX FIFO Interrupt
#define UART_FFRX_RXFFINTCLR      0x0040      // UART RX FIFO Interrupt Clear
#define UART_FFRX_RXFFIENA        0x0020      // UART RX FIFO Interrupt Enable
#define UART_FFRX_RXFFIL_M        0x001F      // UART RX FIFO Interrupt Level

//*****************************************************************************
//
// The following are defines for the bit fields in the UART_O_FFCT register.
//
//*****************************************************************************
#define UART_FFCT_ABD             0x8000      // UART Autobaud Detect
#define UART_FFCT_ABDCLR          0x4000      // UART Autobaud clear
#define UART_FFCT_CDC             0x2000      // UART CDC Calibrate
#define UART_FFCT_FFTXDLY_M       0x00FF      // UART FIFO transfer delay

//*****************************************************************************
//
// The following are defines for the bit fields in the UART_O_PRI register.
//
//*****************************************************************************
#define UART_PRI_SOFT             0x10        // UART Soft
#define UART_PRI_FREE             0x80        // UART Free


#endif // __HW_UART_H__


