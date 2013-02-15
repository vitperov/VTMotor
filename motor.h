/*
 * This file is part of VTMotor (I2C motor driver)
 *
 * Copyright 2012 Vitaly Perov <vitperov@gmail.com>
 * Copyright 2013 Denis Morin <morind79@gmail.com>
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
void go_forward();
void go_back();
void turn_left();
void turn_right();

inline void drv_set_direction(uint8_t left, uint8_t right);
inline void drv_motorType(uint8_t type);
inline void drv_set_speed(uint8_t speed);
inline void drv_set_left_speed(uint8_t speed);
inline void drv_set_right_speed(uint8_t speed);
inline void drv_StepForward();
inline void drv_StepBackward();
inline void drv_StepsForward();
inline void drv_StepsBackward();
inline void drv_StepSpeed(uint8_t speed);
inline void drv_StepRelease();

#endif
