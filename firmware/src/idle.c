/**************************************************************************//**
 * Copyright (c) 2015 by Silicon Laboratories Inc. All rights reserved.
 *
 * http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt
 *****************************************************************************/

#include "si_toolchain.h"
#include "idle.h"

// ----------------------------------------------------------------------------
// Variables
// ----------------------------------------------------------------------------
static idleTimer_TypeDef idle;

// ----------------------------------------------------------------------------
// Functions
// ----------------------------------------------------------------------------

void idleTimerStart(void)
{
  if (idle.rate > 0)
  {
    idle.timer = idle.rate * 4;
  }
  else
  {
    idle.timer = POLL_RATE;
  }
}

bool isIdleTimerIndefinite(void)
{
  if (idle.rate == 0)
  {
    return 1;
  }
  else
  {
    return 0;
  }
}
bool isIdleTimerExpired(void)
{
  if (idle.timer == 0)
  {
    idleTimerStart();
    return 1;
  }
  return 0;
}

void idleTimerSet(uint8_t rate)
{
  if ((rate != 0) && (rate < (POLL_RATE / 4)))
  {
    idle.rate = POLL_RATE / 4;
  }
  else
  {
    idle.rate = rate;
  }

  idleTimerStart();
}

void idleTimerTick(void)
{
  if (idle.timer > 0)
  {
    idle.timer--;
  }
}

uint8_t idleGetRate(void)
{
  return idle.rate;
}
