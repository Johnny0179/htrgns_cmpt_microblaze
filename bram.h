#ifndef BRAM_H
#define BRAM_H
#include <stdio.h>
#include "xbram.h"
#include "xparameters.h"
/************************** Constant Definitions *****************************/

/*
 * The following constants map to the XPAR parameters created in the
 * xparameters.h file. They are defined here such that a user can easily
 * change all the needed parameters in one place.
 */
#define BRAM_DEVICE_ID XPAR_BRAM_1_DEVICE_ID

// BRAM base address
#define BRAM_BASE_ADDR XPAR_AXI_BRAM_CTRL_PL_S_AXI_BASEADDR

/************************** Function Prototypes ******************************/

int BramExample(u16 DeviceId);
void InitializeECC(XBram_Config *ConfigPtr, u32 EffectiveAddr);

#endif
