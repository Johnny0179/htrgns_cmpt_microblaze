#include "uart_polled.h"

/************************** Variable Definitions *****************************/

XUartPs Uart_PS; /* Instance of the UART Device */

/*
 * The following buffers are used in this example to send and receive data
 * with the UART.
 */
static u8 SendBuffer[SIZE]; /* Buffer for Transmitting Data */
static u8 RecvBuffer[SIZE]; /* Buffer for Receiving Data */

/*****************************************************************************/
/**
 *
 * Main function to call the Uart Polled mode example.
 *
 * @param	None
 *
 * @return	XST_SUCCESS if succesful, otherwise XST_FAILURE
 *
 * @note		None
 *
 ******************************************************************************/
#ifndef TESTAPP_GEN
int UartPolledCheck() { return UartPsPolledExample(UART_DEVICE_ID); }
#endif
/*****************************************************************************/
/**
 *
 * This function does a minimal test on the XUartPs device in polled mode.
 *
 * This function sends data and expects to receive the data thru the UART
 * using the local loopback mode.
 *
 *
 * @param	DeviceId is the unique device id from hardware build.
 *
 * @return	XST_SUCCESS if successful, XST_FAILURE if unsuccessful
 *
 * @note
 * This function polls the UART, it may hang if the hardware is not
 * working correctly.
 *
 ****************************************************************************/
int UartPsPolledExample(u16 DeviceId) {
  int Status;
  XUartPs_Config *Config;
  unsigned int SentCount;
  unsigned int ReceivedCount;
  u16 Index;
  u32 LoopCount = 0;

  /*
   * Initialize the UART driver so that it's ready to use.
   * Look up the configuration in the config table, then initialize it.
   */
  Config = XUartPs_LookupConfig(DeviceId);
  if (NULL == Config) {
    return XST_FAILURE+1;
  }

  Status = XUartPs_CfgInitialize(&Uart_PS, Config, Config->BaseAddress);
  if (Status != XST_SUCCESS) {
    return XST_FAILURE+2;
  }

  /* Check hardware build. */
  Status = XUartPs_SelfTest(&Uart_PS);
  if (Status != XST_SUCCESS) {
    return XST_FAILURE+3;
  }

  /* Use local loopback mode. */
  XUartPs_SetOperMode(&Uart_PS, XUARTPS_OPER_MODE_LOCAL_LOOP);

  /*
   * Initialize the send buffer bytes with a pattern and zero out
   * the receive buffer.
   */
  for (Index = 0; Index < SIZE; Index++) {
    SendBuffer[Index] = '0' + Index;
    RecvBuffer[Index] = 0;
  }

  /* Block sending the buffer. */
  SentCount = XUartPs_Send(&Uart_PS, SendBuffer, SIZE);
  if (SentCount != SIZE) {
    return XST_FAILURE+4;
  }

  /*
   * Wait while the UART is sending the data so that we are guaranteed
   * to get the data the 1st time we call receive, otherwise this function
   * may enter receive before the data has arrived
   */
  while (XUartPs_IsSending(&Uart_PS)) {
    LoopCount++;
  }

  /* Block receiving the buffer. */
  ReceivedCount = 0;
  while (ReceivedCount < SIZE) {
    ReceivedCount += XUartPs_Recv(&Uart_PS, &RecvBuffer[ReceivedCount],
                                  (SIZE - ReceivedCount));
  }

  /*
   * Check the receive buffer against the send buffer and verify the
   * data was correctly received
   */
  for (Index = 0; Index < SIZE; Index++) {
    if (SendBuffer[Index] != RecvBuffer[Index]) {
      return XST_FAILURE + 5;
    }
  }

  /* Restore to normal mode. */
  XUartPs_SetOperMode(&Uart_PS, XUARTPS_OPER_MODE_NORMAL);

  return XST_SUCCESS;
}
