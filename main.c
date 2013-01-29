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

#include "motor.h"
#include "external/SMBSlave.h"

/* TIMER */
inline void conf_TMR0()
{
  /* set clock source f_t0 = f_io/8 = 1 MHz */
  TCCR0 = _BV(CS01);

  TIMSK |= _BV(TOIE0); /* Enable TMR0 interrupt */
}

inline void configure()
{
  // Initialize SMBus
  SMBusInit();
  SMBEnable();

  conf_TMR0();
  conf_motors();

  SREG |= _BV(7); /* Enable interrupts */
}

int main(void)
{
  configure();
  while (1);
  return 0;
}
