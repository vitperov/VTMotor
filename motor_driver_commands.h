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

/* I2C commands */
#define DRV_WHO_AM_I        0x10  // Reply DRV_WHO_AM_I_RESPONSE
#define MOTOR_TYPE          0x11  // 0=DC motor, 1=stepper motor
#define DRV_TIMER_DIV       0x12  // Frequency divider 1->8/1=8MHz, 2->8/8=1MHz, 3->8/64=0.125MHz
#define STEP_SPEED          0x13  // Set speed for PWM or stepper (Ex: 50 with DRV_TIMER_DIV=2 -> 200Hz)
#define STEP_REMAINING      0x14  // Return 1 if it is still running 0 otherwise
// DC motor
#define DRV_DRV_ENABLE      0x15
#define DRV_DRV_DISABLE     0x16
#define DRV_SET_SPEED       0x17
#define DRV_SET_DIRECTION   0x18
// Stepper motor
#define STEP_FORWARD        0x1A  // Stepper 1 step forward
#define STEP_BACKWARD       0x1B  // Stepper 1 step backward
#define STEPS_FORWARD       0x1C  // Stepper SET_NUM_STEP steps forward
#define STEPS_BACKWARD      0x1D  // Stepper SET_NUM_STEP steps backward
#define SET_NUM_STEP        0x1E  // Set the number of step 0..254
#define STEP_RELEASE        0x1F  // Set all output to 0 (Free stepper)

#define DRV_DIR_FORWARD     0x00
#define DRV_DIR_BACK        0x01

#define DRV_WHO_AM_I_RESPONSE   0x47
