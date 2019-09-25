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

// shared BRAM ID
#define BRAM_DEVICE_ID XPAR_BRAM_3_DEVICE_ID

// shared BRAM base address
#define BRAM_BASE_ADDR XPAR_BRAM_3_BASEADDR

/************************** Function Prototypes ******************************/

int BramExample(u16 DeviceId);
void InitializeECC(XBram_Config *ConfigPtr, u32 EffectiveAddr);

int BramCheck();

#endif
