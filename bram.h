#ifndef BRAM_H
#define BRAM_H
#include "xparameters.h"
#include "xbram.h"
#include <stdio.h>
/************************** Constant Definitions *****************************/

/*
 * The following constants map to the XPAR parameters created in the
 * xparameters.h file. They are defined here such that a user can easily
 * change all the needed parameters in one place.
 */
#define BRAM_DEVICE_ID		XPAR_BRAM_0_DEVICE_ID


/************************** Function Prototypes ******************************/

int BramExample(u16 DeviceId);
static void InitializeECC(XBram_Config *ConfigPtr, u32 EffectiveAddr);


#endif