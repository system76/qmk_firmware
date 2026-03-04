/*
 *  Copyright (C) 2023  System76
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

/*
 * Key matrix pins
 * ROWS: GPIO pins used for rows, top to bottom
 * COLS: GPIO pins used for columns, left to right
 */
// For use when PBRELAY is low
//#define MATRIX_ROW_PINS { GP26 }
//#define MATRIX_COL_PINS { GP27 }

// For use when PBRELAY is high
#define DIRECT_PINS {{ GP8 }}

/*
 * Diode Direction
 * COL2ROW = COL => Anode (+), ROW => Cathode (-)
 * ROW2COL = ROW => Anode (+), COL => Cathode (-)
 */
#define DIODE_DIRECTION COL2ROW

// https://github.com/qmk/qmk_firmware/blob/master/docs/feature_backlight.md
#define BACKLIGHT_PIN GP13
#define BACKLIGHT_PWM_DRIVER PWMD6
#define BACKLIGHT_PWM_CHANNEL RP2040_PWM_CHANNEL_B
#define BACKLIGHT_BREATHING
#define BACKLIGHT_LEVELS 31
#define BACKLIGHT_PERIOD 1

// EEPROM {
#define EEPROM_SIZE 1024
// TODO: Refactor with new user EEPROM code (coming soon)
#define EEPROM_MAGIC 0x76EC
#define EEPROM_MAGIC_ADDR 64
// Bump this every time we change what we store
// This will automatically reset the EEPROM with defaults
// and avoid loading invalid data from the EEPROM
#define EEPROM_VERSION 0x01
#define EEPROM_VERSION_ADDR (EEPROM_MAGIC_ADDR + 2)
// } EEPROM

// System76 EC {
#define SYSTEM76_EC_EEPROM_CASE_REV_ADDR (EEPROM_VERSION_ADDR + 1)
#define SYSTEM76_EC_EEPROM_CASE_REV_SIZE 4
// } System76 EC
