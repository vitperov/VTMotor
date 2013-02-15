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

#include <avr/io.h>
#include <avr/interrupt.h>

#include "motor.h"
#include "motor_driver_commands.h"

/* Settings */
#define TMR_RELOAD      50 /* PWM frequency = 200 Hz */

/* PINS */
#define PWM_CNT_MAX     100

#define DRV_EN_PIN1     PB1
#define DRV_EN_PIN2     PB2

#define DRV1_PWM_PIN    PC0
#define DRV1_DIR_PIN    PC1
#define DRV2_PWM_PIN    PC2
#define DRV2_DIR_PIN    PC3

#define DRV_EN_PORT     PORTB
#define DRV_PWM_PORT    PORTC
#define DRV_DIR_PORT    PORTC

#define DRV_EN_DDR      DDRB
#define DRV_PWM_DDR     DDRC
#define DRV_DIR_DDR     DDRC

/* internal variables */

/* Stepper info */
static const uint8_t seqWaveDrive[4] = {0b1000, 0b0010, 0b0100, 0b0001};
static const uint8_t seqFullStep[4] = {0b1010, 0b0110, 0b0101, 0b1001};

static uint8_t dir1_is_forward, dir2_is_forward;
static uint8_t drv_enabled;
static uint8_t speed1, speed2, mType;
static uint8_t sCmd, mSpeed, stepOk, was;
static uint8_t pwm_cnt;
static uint16_t numStep, stp_cnt;

/********************
 * HELPER FUNCTIONS *
 ********************/

static void drv1_forward()
{
  DRV_DIR_PORT &= ~_BV(DRV1_DIR_PIN);
  dir1_is_forward = 1;
}

static void drv1_back()
{
  DRV_DIR_PORT |= _BV(DRV1_DIR_PIN);
  dir1_is_forward = 0;
}

static void drv2_forward()
{
  DRV_DIR_PORT &= ~_BV(DRV2_DIR_PIN);
  dir2_is_forward = 1;
}

static void drv2_back()
{
  DRV_DIR_PORT |= _BV(DRV2_DIR_PIN);
  dir2_is_forward = 0;
}

static inline void drv1_turn_on()
{
  if (dir1_is_forward)
    DRV_PWM_PORT |= _BV(DRV1_PWM_PIN);
  else
    DRV_PWM_PORT &= ~_BV(DRV1_PWM_PIN);
}

static inline void drv1_turn_off()
{
  if (dir1_is_forward)
    DRV_PWM_PORT &= ~_BV(DRV1_PWM_PIN);
  else
    DRV_PWM_PORT |= _BV(DRV1_PWM_PIN);
}

static inline void drv2_turn_on()
{
  if (dir2_is_forward)
    DRV_PWM_PORT |= _BV(DRV2_PWM_PIN);
  else
    DRV_PWM_PORT &= ~_BV(DRV2_PWM_PIN);
}

static inline void drv2_turn_off()
{
  if (dir2_is_forward)
    DRV_PWM_PORT &= ~_BV(DRV2_PWM_PIN);
  else
    DRV_PWM_PORT |= _BV(DRV2_PWM_PIN);
}

static void conf_motor_pins()
{
  /* Set pins to output */
  DRV_PWM_DDR |= _BV(DRV1_PWM_PIN);
  DRV_DIR_DDR |= _BV(DRV1_DIR_PIN);
  DRV_PWM_DDR |= _BV(DRV2_PWM_PIN);
  DRV_DIR_DDR |= _BV(DRV2_DIR_PIN);
  DRV_EN_DDR  |= _BV(DRV_EN_PIN1);
  DRV_EN_DDR  |= _BV(DRV_EN_PIN2);

  /* Set 0 to all outputs */
  drv1_forward();
  drv2_forward();
  drv1_turn_off();
  drv2_turn_off();
  drv_disable();
}


/********************
 * PUBLIC FUNCTIONS *
 ********************/

void conf_motors()
{
  conf_motor_pins();
  speed1 = 0;
  speed2 = 0;
  pwm_cnt = 0;
  mType = 0;
  mSpeed = 100;
  sCmd = 0;
  numStep = 0;
  stp_cnt = 0;
  stepOk = 0;
  was = 0;
}

uint8_t step_remaining()
{
  if (numStep>0)
    return(1);
  else
    return(0);
}

void inline dc_motors_pwm()
{
  if (!pwm_cnt)
  {
    if (speed1)
      drv1_turn_on();
    if (speed2)
      drv2_turn_on();
  }

  if (pwm_cnt == speed1)
    drv1_turn_off();
  if (pwm_cnt == speed2)
    drv2_turn_off();

  pwm_cnt++;
  if (pwm_cnt > PWM_CNT_MAX)
    pwm_cnt = 0;
}

void inline stepper_motor()
{
  if (stepOk == 1)
    drv_StepForward();
  else if (stepOk == 2)
    drv_StepBackward();
  stp_cnt++;
  if (stp_cnt >= numStep) 
  {
    stepOk = 0;
    numStep = 0;
    stp_cnt = 0;
  }
}

void drv_enable()
{
  DRV_EN_PORT |= _BV(DRV_EN_PIN1);
  DRV_EN_PORT |= _BV(DRV_EN_PIN2);
  drv_enabled = 1;
}

void drv_disable()
{
  DRV_EN_PORT &= ~_BV(DRV_EN_PIN1);
  DRV_EN_PORT &= ~_BV(DRV_EN_PIN2);
  drv_enabled = 0;
}

inline void drv_set_speed(uint8_t speed)
{
  speed1 = speed;
  speed2 = speed;
}

inline void drv_MotorType(uint8_t type)
{
  mType = type;
}

inline void drv_set_left_speed(uint8_t speed)
{
  speed1 = speed;
}

inline void drv_set_right_speed(uint8_t speed)
{
  speed2 = speed;
}

inline void drv_set_direction(uint8_t left, uint8_t right)
{
  if (DRV_DIR_FORWARD == left)
    drv1_forward();
  else
    drv1_back();

  if (DRV_DIR_FORWARD == right)
    drv2_forward();
  else
    drv2_back();
}

void go_forward()
{
  drv1_forward();
  drv2_forward();
}

void go_back()
{
  drv1_back();
  drv2_back();
}

void turn_left()
{
  drv1_back();
  drv2_forward();
}

void turn_right()
{
  drv2_back();
  drv1_forward();
}

inline void drv_TimerType(uint8_t type)
{
  disableTimer();
  TCCR0 = 0;
  TCCR0 |= type;
  enableTimer();
}

ISR (TIMER0_OVF_vect)
{
  if(mType==0)
  {
    dc_motors_pwm();
  }
  else
  {
    stepper_motor();
  }

  TCNT0 -= mSpeed;
}

// Stepper

void setIn1()
{
  DRV_PWM_PORT |= _BV(DRV1_PWM_PIN);
}

void unsetIn1()
{
  DRV_PWM_PORT &= ~_BV(DRV1_PWM_PIN);
}

void setIn2()
{
  DRV_DIR_PORT |= _BV(DRV1_DIR_PIN);
}

void unsetIn2()
{
  DRV_DIR_PORT &= ~_BV(DRV1_DIR_PIN);
}

void setIn3()
{
  DRV_PWM_PORT |= _BV(DRV2_PWM_PIN);
}

void unsetIn3()
{
  DRV_PWM_PORT &= ~_BV(DRV2_PWM_PIN);
}

void setIn4()
{
  DRV_DIR_PORT |= _BV(DRV2_DIR_PIN);
}

void unsetIn4()
{
  DRV_DIR_PORT &= ~_BV(DRV2_DIR_PIN);
}

void stepperInit()
{
  // Everything set to 0
  unsetIn1();
  unsetIn2();
  unsetIn3();
  unsetIn4();
}

void waveDrive(uint8_t thisStep)
{
  if (seqWaveDrive[thisStep] & 0b1000) setIn1(); else unsetIn1();
  if (seqWaveDrive[thisStep] & 0b0100) setIn2(); else unsetIn2();
  if (seqWaveDrive[thisStep] & 0b0010) setIn3(); else unsetIn3();
  if (seqWaveDrive[thisStep] & 0b0001) setIn4(); else unsetIn4();
}

void fullStep(uint8_t thisStep)
{
  if (seqFullStep[thisStep] & 0b1000) setIn1(); else unsetIn1();
  if (seqFullStep[thisStep] & 0b0100) setIn2(); else unsetIn2();
  if (seqFullStep[thisStep] & 0b0010) setIn3(); else unsetIn3();
  if (seqFullStep[thisStep] & 0b0001) setIn4(); else unsetIn4();
}

void drv_SetNumStep(uint8_t num1, uint8_t num2)
{
  numStep = num1 * num2;
}

void drv_StepForward()
{
  if (mType>0)
  {
    if (was==2) sCmd++;
    was=1;
    // Step the motor one step
    fullStep(sCmd % 4);
    sCmd++;
    if (sCmd>3) sCmd=0;
  }
}

void drv_StepBackward()
{
  if (mType>0)
  {
    if (was==1) sCmd--;
    was=2;
    // Step the motor one step
    fullStep(sCmd % 4);
    sCmd--;
    if (sCmd<0) sCmd=3;
  }	
}

void drv_StepsForward()
{
  stepOk = 1;
}

void drv_StepsBackward()
{
  stepOk = 2;
}

void drv_StepSpeed(uint8_t speed)
{
  mSpeed = speed;
}

void drv_StepRelease()
{
  stepperInit();
}





