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

/* I2C commands */
#define DRV_WHO_AM_I        0x10
#define DRV_DRV_ENABLE      0x11
#define DRV_DRV_DISABLE     0x12
#define DRV_SET_SPEED       0x13
#define DRV_SET_DIRECTION   0x14

#define DRV_DIR_FORWARD     0x0
#define DRV_DIR_BACK        0x1

#define DRV_WHO_AM_I_RESPONSE   0x47
