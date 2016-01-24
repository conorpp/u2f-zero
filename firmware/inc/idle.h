/**************************************************************************//**
 * Copyright (c) 2015 by Silicon Laboratories Inc. All rights reserved.
 *
 * http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt
 *****************************************************************************/

#include "si_toolchain.h"
#include <stdint.h>
#include <stdbool.h>

// ----------------------------------------------------------------------------
// Constants
// ----------------------------------------------------------------------------
#define DEFAULT_IDLE_RATE       500     // Rate defined in the HID class doc.
#define POLL_RATE               24      // The bInterval reported with the
                                        // interrupt IN endpoint descriptor.

// ----------------------------------------------------------------------------
// Typedefs
// ----------------------------------------------------------------------------
typedef struct
{
  uint8_t rate;
  uint16_t timer;
} idleTimer_TypeDef;

// ----------------------------------------------------------------------------
// Function Prototypes
// ----------------------------------------------------------------------------
void idleTimerStart(void);
bool isIdleTimerIndefinite(void);
bool isIdleTimerExpired(void);
void idleTimerSet(uint8_t rate);
void idleTimerTick(void);
uint8_t idleGetRate(void);
