#include "can_polled.h"

/************************** Variable Definitions *****************************/

/*
 * Buffers to hold frames to send and receive. These are declared as global so
 * that they are not on the stack.
 * These buffers need to be 32-bit aligned
 */
static u32 TxFrame[XCANPS_MAX_FRAME_SIZE_IN_WORDS];
static u32 RxFrame[XCANPS_MAX_FRAME_SIZE_IN_WORDS];

/* Driver instance */
static XCanPs Can;

/****************************************************************************/
/**
 *
 * This function is the main function of the Can polled example.
 *
 * @param	None
 *
 * @return
 *		- XST_SUCCESS if the example has completed successfully.
 *		- XST_FAILURE if the example has failed.
 *
 * @note		None
 *
 *****************************************************************************/
#ifndef TESTAPP_GEN
int CanPsPolledCheck() { return CanPsPolledExample(CAN_DEVICE_ID); }
#endif

/*****************************************************************************/
/**
 *
 * The entry point for showing the XCanPs driver in polled mode. The example
 * configures the device for internal loop back mode, then sends a Can
 * frame, receives the same Can frame, and verifies the frame contents.
 *
 * @param	DeviceId is the XPAR_<CANPS_instance>_DEVICE_ID value from
 *		xparameters.h
 *
 * @return	XST_SUCCESS if successful, otherwise driver-specific error code.
 *
 * @note
 *
 * If the device is not working correctly, this function may enter an infinite
 * loop and will never return to the caller.
 *
 ******************************************************************************/
int CanPsPolledExample(u16 DeviceId) {
  int Status;
  XCanPs *CanInstPtr = &Can;
  XCanPs_Config *ConfigPtr;

  /*
   * Initialize the Can device.
   */
  ConfigPtr = XCanPs_LookupConfig(DeviceId);
  if (CanInstPtr == NULL) {
    return XST_FAILURE + 1;
  }
  Status = XCanPs_CfgInitialize(CanInstPtr, ConfigPtr, ConfigPtr->BaseAddr);
  if (Status != XST_SUCCESS) {
    return XST_FAILURE + 2;
  }

  /*
   * Run self-test on the device, which verifies basic sanity of the
   * device and the driver.
   */
  Status = XCanPs_SelfTest(CanInstPtr);
  if (Status != XST_SUCCESS) {
    return XST_FAILURE + 3;
  }

  /*
   * Enter Configuration Mode so we can setup Baud Rate Prescaler
   * Register (BRPR) and Bit Timing Register (BTR).
   */
  XCanPs_EnterMode(CanInstPtr, XCANPS_MODE_CONFIG);
  while (XCanPs_GetMode(CanInstPtr) != XCANPS_MODE_CONFIG)
    ;

  /*
   * Setup Baud Rate Prescaler Register (BRPR) and
   * Bit Timing Register (BTR).
   */
  XCanPs_SetBaudRatePrescaler(CanInstPtr, TEST_BRPR_BAUD_PRESCALAR);
  XCanPs_SetBitTiming(CanInstPtr, TEST_BTR_SYNCJUMPWIDTH,
                      TEST_BTR_SECOND_TIMESEGMENT,

                      TEST_BTR_FIRST_TIMESEGMENT);

  /*
   * Enter Loop Back Mode.
   */
  XCanPs_EnterMode(CanInstPtr, XCANPS_MODE_LOOPBACK);
  while (XCanPs_GetMode(CanInstPtr) != XCANPS_MODE_LOOPBACK)
    ;

  /*
   * Send a frame, receive the frame via the loop back and verify its
   * contents.
   */
  Status = SendFrame(CanInstPtr);
  if (Status != XST_SUCCESS) {
    return Status;
  }

  Status = RecvFrame(CanInstPtr);

  return Status;
}

/*****************************************************************************/
/**
 *
 * Send a CAN frame.
 *
 * @param	InstancePtr is a pointer to the driver instance
 *
 * @return	XST_SUCCESS if successful, a driver-specific return code if not.
 *
 * @note
 *
 * This function waits until TX FIFO has room for at least one frame before
 * sending a frame. So this function may block if the hardware is not built
 * correctly.
 *
 ******************************************************************************/
int SendFrame(XCanPs *InstancePtr) {
  u8 *FramePtr;
  int Index;
  int Status;

  /*
   * Create correct values for Identifier and Data Length Code Register.
   */
  TxFrame[0] = (u32)XCanPs_CreateIdValue((u32)TEST_MESSAGE_ID, 0, 0, 0, 0);
  TxFrame[1] = (u32)XCanPs_CreateDlcValue((u32)FRAME_DATA_LENGTH);

  /*
   * Now fill in the data field with known values so we can verify them
   * on receive.
   */
  FramePtr = (u8 *)(&TxFrame[2]);
  for (Index = 0; Index < FRAME_DATA_LENGTH; Index++) {
    *FramePtr++ = (u8)Index;
  }

  /*
   * Wait until TX FIFO has room.
   */
  while (XCanPs_IsTxFifoFull(InstancePtr) == TRUE)
    ;

  /*
   * Now send the frame.
   *
   * Another way to send a frame is keep calling XCanPs_Send() until it
   * returns XST_SUCCESS. No check on if TX FIFO is full is needed anymore
   * in that case.
   */
  Status = XCanPs_Send(InstancePtr, TxFrame);

  return Status;
}

/*****************************************************************************/
/**
 *
 * This function receives a frame and verifies its contents.
 *
 * @param	InstancePtr is a pointer to the driver instance.
 *
 * @return	XST_SUCCESS if successful, a driver-specific return code if not.
 *
 * @note
 *
 * This function waits until RX FIFO becomes not empty before reading a frame
 * from it. So this function may block if the hardware is not built
 * correctly.
 *
 ******************************************************************************/
int RecvFrame(XCanPs *InstancePtr) {
  u8 *FramePtr;
  int Status;
  int Index;

  /*
   * Wait until a frame is received.
   */
  while (XCanPs_IsRxEmpty(InstancePtr) == TRUE)
    ;

  /*
   * Receive a frame and verify its contents.
   */
  Status = XCanPs_Recv(InstancePtr, RxFrame);
  if (Status == XST_SUCCESS) {
    /*
     * Verify Identifier and Data Length Code.
     */
    if (RxFrame[0] !=
        (u32)XCanPs_CreateIdValue((u32)TEST_MESSAGE_ID, 0, 0, 0, 0))
      return XST_LOOPBACK_ERROR;

    if ((RxFrame[1] & ~XCANPS_DLCR_TIMESTAMP_MASK) != TxFrame[1])
      return XST_LOOPBACK_ERROR;

    /*
     * Verify Data field contents.
     */
    FramePtr = (u8 *)(&RxFrame[2]);
    for (Index = 0; Index < FRAME_DATA_LENGTH; Index++) {
      if (*FramePtr++ != (u8)Index) {
        return XST_LOOPBACK_ERROR;
      }
    }
  }

  return Status;
}
