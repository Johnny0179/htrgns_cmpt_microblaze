/******************************************************************************
 *
 * Copyright (C) 2010 - 2014 Xilinx, Inc.  All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * Use of the Software is limited solely to applications:
 * (a) running on a Xilinx device, or
 * (b) that interact with a Xilinx device through a bus or interconnect.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * XILINX  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * Except as contained in this notice, the name of the Xilinx shall not be used
 * in advertising or otherwise to promote the sale, use or other dealings in
 * this Software without prior written authorization from Xilinx.
 *
 ******************************************************************************/
/****************************************************************************/
/**
 *
 * @file		xuartps_intr_example.c
 *
 * This file contains a design example using the XUartPs driver in interrupt
 * mode. It sends data and expects to receive the same data through the device
 * using the local loopback mode.
 *
 *
 * @note
 * The example contains an infinite loop such that if interrupts are not
 * working it may hang.
 *
 * MODIFICATION HISTORY:
 * <pre>
 * Ver   Who    Date     Changes
 * ----- ------ -------- ----------------------------------------------
 * 1.00a  drg/jz 01/13/10 First Release
 * 1.00a  sdm    05/25/11 Modified the example for supporting Peripheral tests
 *		        in SDK
 * 1.03a  sg     07/16/12 Updated the example for CR 666306. Modified
 *			the device ID to use the first Device Id
 *			and increased the receive timeout to 8
 *			Removed the printf at the start of the main
 *			Put the device normal mode at the end of the example
 * 3.1	kvn		04/10/15 Added code to support Zynq Ultrascale+ MP.
 * 3.1   mus     01/14/16 Added support for intc interrupt controller
 *
 * </pre>
 ****************************************************************************/

/***************************** Include Files *******************************/

#include "xil_exception.h"
#include "xil_printf.h"
#include "xparameters.h"
#include "xplatform_info.h"
#include "xuartps.h"

#ifdef XPAR_INTC_0_DEVICE_ID
#include "xintc.h"
#else
#include "xscugic.h"
#endif
/************************** Constant Definitions **************************/

/*
 * The following constants map to the XPAR parameters created in the
 * xparameters.h file. They are defined here such that a user can easily
 * change all the needed parameters in one place.
 */
#ifdef XPAR_INTC_0_DEVICE_ID
#define INTC XIntc
#define UART_DEVICE_ID XPAR_XUARTPS_0_DEVICE_ID
#define INTC_DEVICE_ID XPAR_INTC_0_DEVICE_ID
#define UART_INT_IRQ_ID XPAR_INTC_0_UARTPS_0_VEC_ID
#else
#define INTC XScuGic
#define UART_DEVICE_ID XPAR_XUARTPS_0_DEVICE_ID
#define INTC_DEVICE_ID XPAR_SCUGIC_SINGLE_DEVICE_ID
#define UART_INT_IRQ_ID XPAR_XUARTPS_1_INTR
#endif
/*
 * The following constant controls the length of the buffers to be sent
 * and received with the UART,
 */
#define TEST_BUFFER_SIZE 10
#define LASER_BUF_SIZE 1
/**************************** Type Definitions ******************************/

/************************** Function Prototypes *****************************/

int UartPsIntrExample(INTC *IntcInstPtr, XUartPs *UartInstPtr, u16 DeviceId,
                      u16 UartIntrId);

int SetupInterruptSystem(INTC *IntcInstancePtr, XUartPs *UartInstancePtr,
                                u16 UartIntrId);

void Handler(void *CallBackRef, u32 Event, unsigned int EventData);

int UartCheck();
