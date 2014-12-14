/*
 * This file is part of VTMotor (I2C motor driver)
 *
 * Copyright 2012 Vitaly Perov <vitperov@gmail.com>
 *
 * VTMotor is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with VTMotor.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <avr/io.h> 
#include <avr/pgmspace.h>

#include "smbus_commands.h"
#include "motor_driver_commands.h"
#include "motor.h"

static void WhoAmI(SMBData *smb);
static void EnableMotors(SMBData *smb);
static void DisableMotors(SMBData *smb);
static void SetSpeed(SMBData *smb);
static void SetDerection(SMBData *smb);
static void UndefinedCommand(SMBData *smb);
static void UndefinedCommand(SMBData *smb);


void ProcessReceiveByte(SMBData *smb)
{
    smb->txBuffer[0] = ~PIND;
    smb->txLength = 1;
}

void ProcessMessage(SMBData *smb)
{
  if (smb->state != SMB_STATE_WRITE_REQUESTED)
  {
    smb->state = SMB_STATE_IDLE;
    return;
  }

  switch (smb->rxBuffer[0]) // Command code.
  {
  case DRV_WHO_AM_I:
    WhoAmI(smb);
    break;
  case DRV_SET_DIRECTION:
    SetDerection(smb);
    break;
  case DRV_SET_SPEED:
    SetSpeed(smb);
    break;
  case DRV_DRV_ENABLE:
    EnableMotors(smb);
    break;
  case DRV_DRV_DISABLE:
    DisableMotors(smb);
    break;
  default:
    UndefinedCommand(smb);
  break;
  }
}

static inline void WhoAmI(SMBData *smb)
{
  smb->txBuffer[0] = DRV_WHO_AM_I_RESPONSE;
  smb->txLength = 1;
  smb->state = SMB_STATE_WRITE_READ_REQUESTED;
}

char cmd = 0;

static void inline EnableMotors(SMBData *smb)
{
  if (smb->rxCount != 1)
  {
    smb->error = TRUE;
    return;
  }

  drv_enable();

  smb->state = SMB_STATE_IDLE;
}

static void inline DisableMotors(SMBData *smb)
{
  if (smb->rxCount != 1)
  {
    smb->error = TRUE;
    return;
  }

  drv_disable();

  smb->state = SMB_STATE_IDLE;
}

static inline void SetSpeed(SMBData *smb)
{
  if (smb->rxCount != 3)
  {
    smb->error = TRUE;
    return;
  }

  drv_set_speed(smb->rxBuffer[1], smb->rxBuffer[2]);

  smb->state = SMB_STATE_IDLE;
}

static inline void SetDerection(SMBData *smb)
{
  if (smb->rxCount != 3)
  {
    smb->error = TRUE;
    return;
  }

  drv_set_direction(smb->rxBuffer[1], smb->rxBuffer[2]);

  smb->state = SMB_STATE_IDLE;
}

static inline void UndefinedCommand(SMBData *smb)
{
  // Handle undefined requests here.
  smb->error = TRUE;
  smb->state = SMB_STATE_IDLE;
}
