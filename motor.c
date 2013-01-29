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
static uint8_t dir1_is_forward, dir2_is_forward;
static uint8_t drv_enabled;
static uint8_t speed1, speed2;
static uint8_t pwm_cnt;

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
}

void inline motors_pwm()
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

ISR (TIMER0_OVF_vect)
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


  TCNT0 -= TMR_RELOAD;
}

