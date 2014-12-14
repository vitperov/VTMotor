/*
 * This file is part of VTMotor (I2C motor driver)
 *
 * Copyright 2012-2014 Vitaly Perov <vitperov@gmail.com>
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

#ifndef _MOTOR_H
#define _MOTOR_H

void conf_motors();
void inline motors_pwm();

void drv_enable();
void drv_disable();

inline void drv_set_direction(uint8_t left, uint8_t right);
inline void drv_set_speed    (uint8_t left, uint8_t right);

#endif
