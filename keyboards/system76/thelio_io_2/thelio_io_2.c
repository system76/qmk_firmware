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

#include "analog.h"
#include "config.h"
#include "eeprom.h"
#include "../system76_ec.h"

#include "thelio_io_2.h"

#define TACH0 GP5
#define TACH1 GP7
#define PBSENSE GP8
#define TACH2 GP9
#define TACH3 GP11
#define DIS_PWMIN GP12
#define PLED GP13
#define PWM0 GP16
#define PWM1 GP17
#define PWM2 GP18
#define PWM3 GP19
#define PBRELAY GP22
#define ADC0 GP26
#define ADC1 GP27
#define ADC2 GP28
#define ADC3 GP29

// True when driver has suspended
static bool driver_suspended = false;

// True when USB is suspended
static bool usb_suspended = false;

// Debounce time, in ms
#define DEBOUNCE_TIMEOUT 15

// Time before MB LED turns off so suspend status can be determined, in ms
#define MB_LED_OFF_TIMEOUT 2000

#define FAN_TIMEOUT 5000
static uint32_t fan_time = 0;

#define UNLOCK_TIMEOUT 15000
static uint32_t unlock_time = 0;

enum PowerState {
    POWER_STATE_OFF,
    POWER_STATE_SUSPEND,
    POWER_STATE_ON,
};

const static PWMConfig PWM_CFG = {
    .frequency = 6375000, /* PWM clock frequency, 25KHz, multiplied by 255 ticks */
    .period    = 254, /* PWM period in counter ticks minus one. e.g. clock frequency is 25KHz, period is 255 ticks then t_period is 40us */
    .channels[RP2040_PWM_CHANNEL_A - 1].mode = PWM_OUTPUT_ACTIVE_LOW,
    .channels[RP2040_PWM_CHANNEL_B - 1].mode = PWM_OUTPUT_ACTIVE_LOW,
};

struct Fan {
    PWMDriver * pwm_drv;
    pwmchannel_t pwm_chan;
    int pwm_gpio;
    int tach_gpio;
    uint8_t duty;
    uint32_t duty_time;
    uint16_t tach;
    uint8_t tach_state;
    uint16_t tach_edge;
    uint32_t tach_time;
};

static struct Fan FANOUT1 = {
    .pwm_drv = &PWMD0,
    .pwm_chan = RP2040_PWM_CHANNEL_A,
    .pwm_gpio = PWM0,
    .tach_gpio = TACH0,
    .duty = 0,
    .tach = 0,
    .tach_state = 0,
    .tach_edge = 0,
    .tach_time = 0,
};

static struct Fan FANOUT2 = {
    .pwm_drv = &PWMD0,
    .pwm_chan = RP2040_PWM_CHANNEL_B,
    .pwm_gpio = PWM1,
    .tach_gpio = TACH1,
    .duty = 0,
    .tach = 0,
    .tach_state = 0,
    .tach_edge = 0,
    .tach_time = 0,
};

static struct Fan FANOUT3 = {
    .pwm_drv = &PWMD1,
    .pwm_chan = RP2040_PWM_CHANNEL_A,
    .pwm_gpio = PWM2,
    .tach_gpio = TACH2,
    .duty = 0,
    .tach = 0,
    .tach_state = 0,
    .tach_edge = 0,
    .tach_time = 0,
};

static struct Fan FANOUT4 = {
    .pwm_drv = &PWMD1,
    .pwm_chan = RP2040_PWM_CHANNEL_B,
    .pwm_gpio = PWM3,
    .tach_gpio = TACH3,
    .duty = 0,
    .tach = 0,
    .tach_state = 0,
    .tach_edge = 0,
    .tach_time = 0,
};

void fan_enable_pwm(struct Fan * fan) {
    pwmEnableChannel(fan->pwm_drv, fan->pwm_chan - 1, fan->duty);
    palSetLineMode(fan->pwm_gpio, BACKLIGHT_PAL_MODE);
}

void fan_disable_pwm(struct Fan * fan) {
    setPinInput(fan->pwm_gpio);
    pwmDisableChannel(fan->pwm_drv, fan->pwm_chan - 1);
}

void fan_init(struct Fan * fan) {
    fan_disable_pwm(fan);

    setPinInputHigh(fan->tach_gpio);
    fan->tach_state = readPin(fan->tach_gpio);
}

void fan_scan(struct Fan * fan, uint32_t time) {
    uint8_t tach_state = readPin(fan->tach_gpio);
    if (tach_state != fan->tach_state) {
        fan->tach_edge += 1;
        fan->tach_state = tach_state;
    }

    if (TIMER_DIFF_32(time, fan->tach_time) >= 1000) {
        // Each rotation of the fan has two pulses. Each pulse has two edges.
        // There are four edges per rotation. RPM is therefore:
        // 60 * edges per second / 4, or 15 * edges per second
        fan->tach = 15 * fan->tach_edge;
        fan->tach_edge = 0;
        fan->tach_time = time;
    }
}

void keyboard_pre_init_kb(void) {
    // PBRELAY must be high to capture power button presses
    setPinOutput(PBRELAY);
    writePinLow(PBRELAY);

    // DIS_PWMIN must be high to control fans
    setPinOutput(DIS_PWMIN);
    writePinLow(DIS_PWMIN);

    // Start PWM devices used for fans
    pwmStart(&PWMD0, &PWM_CFG);
    pwmStart(&PWMD1, &PWM_CFG);

    // Initialize fans
    fan_init(&FANOUT1);
    fan_init(&FANOUT2);
    fan_init(&FANOUT3);
    fan_init(&FANOUT4);

    // Call user pre_init function
    keyboard_pre_init_user();
}

bool eeprom_is_valid(void) {
    return (
        eeprom_read_word(((void *)EEPROM_MAGIC_ADDR)) == EEPROM_MAGIC &&
        eeprom_read_byte(((void *)EEPROM_VERSION_ADDR)) == EEPROM_VERSION
    );
}
// clang-format on

void eeprom_set_valid(bool valid) {
    eeprom_update_word(((void *)EEPROM_MAGIC_ADDR), valid ? EEPROM_MAGIC : 0xFFFF);
    eeprom_update_byte(((void *)EEPROM_VERSION_ADDR), valid ? EEPROM_VERSION : 0xFF);
}

void matrix_init_kb(void) {
    if (!eeprom_is_valid()) {
        uint8_t rev[SYSTEM76_EC_EEPROM_CASE_REV_SIZE] = { 0 };
        system76_ec_case_rev(rev, SYSTEM76_EC_EEPROM_CASE_REV_SIZE, true);
        eeprom_set_valid(true);
    }
}

void keyboard_post_init_kb(void) {
#ifdef CONSOLE_ENABLE
    // Set console verbosity
    debug_enable   = true;
    debug_matrix   = false;
    debug_keyboard = false;
#endif // CONSOLE_ENABLE

    // Set backlight defaults
    backlight_level_noeeprom(0);
    breathing_disable();

    // Call user post_init function
    keyboard_post_init_user();
}

// Handle fans
static void fan_task(uint32_t time) {
    // Scan fans for tachometer
    fan_scan(&FANOUT1, time);
    fan_scan(&FANOUT2, time);
    fan_scan(&FANOUT3, time);
    fan_scan(&FANOUT4, time);

    // Check if fans are controlled by this device
    if (readPin(DIS_PWMIN)) {
        // If fan timeout reached
        if (TIMER_DIFF_32(time, fan_time) >= FAN_TIMEOUT) {
            printf("Disable fan control\n");
            // Disable fan control by this device
            fan_disable_pwm(&FANOUT1);
            fan_disable_pwm(&FANOUT2);
            fan_disable_pwm(&FANOUT3);
            fan_disable_pwm(&FANOUT4);
            writePinLow(DIS_PWMIN);
        }
    }
}

// Handle power button and power LED
static void power_task(uint32_t time) {
    int16_t adc0 = analogReadPin(ADC0);
    int16_t adc1 = analogReadPin(ADC1);
    int16_t adc2 = analogReadPin(ADC2);
    int16_t adc3 = analogReadPin(ADC3);

    int16_t mv0 = (int16_t)(((3300 * 4 * (int32_t)adc0) / 1024));
    int16_t mv1 = (int16_t)(((3300 * 4 * (int32_t)adc1) / 1024));
    int16_t mv2 = (int16_t)(((3300 * 4 * (int32_t)adc2) / 1024));
    int16_t mv3 = (int16_t)(((3300 * 4 * (int32_t)adc3) / 1024));

    // Deterine the current FP button state based on ADC readings, debounced
    static bool fp_btn = false;
    static uint32_t fp_btn_time = 0;
    bool next_fp_btn = abs(mv1 - mv0) < 1000;
    if ((next_fp_btn != fp_btn) && (TIMER_DIFF_32(time, fp_btn_time) >= DEBOUNCE_TIMEOUT)) {
        printf("FP BTN %d\n", next_fp_btn);
        fp_btn = next_fp_btn;
        fp_btn_time = time;
    }

    // Determine the current MB LED state based on ADC readings, debounced
    static bool mb_led = false;
    static uint32_t mb_led_time = 0;
    bool next_mb_led = abs(mv3 - mv2) > 2000;
    if ((next_mb_led != mb_led) && (TIMER_DIFF_32(time, mb_led_time) >= DEBOUNCE_TIMEOUT)) {
        printf("MB LED %d\n", next_mb_led);
        mb_led = next_mb_led;
        mb_led_time = time;
    }

    // Determine the power state (off, on, suspended)
    static enum PowerState last_power_state = POWER_STATE_OFF;
    enum PowerState power_state = last_power_state;
    if (driver_suspended) {
        power_state = POWER_STATE_SUSPEND;
    } else if (mb_led) {
        power_state = POWER_STATE_ON;
    } else {
        power_state = POWER_STATE_OFF;
    }

    // Apply power state to LED
    static uint8_t bl_level = 0;
    static bool bl_breathing = false;
    if (power_state != last_power_state) {
        printf("Power state %d\n", power_state);
        last_power_state = power_state;

        switch (power_state) {
            case POWER_STATE_OFF:
                bl_level = 0;
                bl_breathing = false;
                break;
            case POWER_STATE_SUSPEND:
                bl_level = BACKLIGHT_LEVELS;
                bl_breathing = true;
                break;
            case POWER_STATE_ON:
                bl_level = BACKLIGHT_LEVELS;
                bl_breathing = false;
                break;
        }
    }
    if (bl_level != get_backlight_level()) {
        backlight_level_noeeprom(bl_level);
    }
    if (bl_breathing != is_breathing()) {
        if (bl_breathing) {
            breathing_enable();
        } else {
            breathing_disable();
        }
    }
}

// Handle power button relay and firmware unlock
static void unlock_task(uint32_t time) {
    // Check if trying to unlock
    if (readPin(PBRELAY)) {
        uint32_t unlock_elapsed = TIMER_DIFF_32(time, unlock_time);
        if (
            // If not unlocked
            !system76_ec_is_unlocked() &&
            // If unlock timer not expired
            (unlock_elapsed <= UNLOCK_TIMEOUT)
        ) {
            backlight_set(((time % 1000) * (BACKLIGHT_LEVELS + 1)) / 1000);
        } else {
            // Disable power button override
            writePinLow(PBRELAY);
            backlight_set(get_backlight_level());
        }
    }

    if (system76_ec_is_unlocked()) {
        backlight_set(BACKLIGHT_LEVELS - ((time % 1000) * (BACKLIGHT_LEVELS + 1)) / 1000);
    }
}

// This function is called repeatedly while the system is awake
void housekeeping_task_kb(void) {
    uint32_t time = timer_read32();

    fan_task(time);
    power_task(time);
    unlock_task(time);
}

// This function is called when the system wakes
void suspend_wakeup_init_kb(void) {
    usb_suspended = false;
}

// This function is called repeatedly when the system has suspended USB
void suspend_power_down_kb(void) {
    usb_suspended = true;

    // Run housekeeping task (which is blocked while suspended)
    housekeeping_task_kb();
}

bool process_record_kb(uint16_t keycode, keyrecord_t *record) {
    if (input_disabled) {
        return false;
    }

    if (!process_record_user(keycode, record)) {
        return false;
    }

    switch (keycode) {
        case QK_BOOT:
            if (!record->event.pressed) {
                system76_ec_unlock();
            }
            return false;
    }

    return true;
}

bool system76_ec_fan_get(uint8_t index, uint8_t * duty) {
    switch (index) {
        case 0:
            *duty = FANOUT1.duty;
            return true;
        case 1:
            *duty = FANOUT2.duty;
            return true;
        case 2:
            *duty = FANOUT3.duty;
            return true;
        case 3:
            *duty = FANOUT4.duty;
            return true;
        default:
            return false;
    }
}

bool system76_ec_fan_set(uint8_t index, uint8_t duty) {
    switch (index) {
        case 0:
            FANOUT1.duty = duty;
            break;
        case 1:
            FANOUT2.duty = duty;
            break;
        case 2:
            FANOUT3.duty = duty;
            break;
        case 3:
            FANOUT4.duty = duty;
            break;
        default:
            return false;
    }

    // Enable fan control by this device and set time
    fan_enable_pwm(&FANOUT1);
    fan_enable_pwm(&FANOUT2);
    fan_enable_pwm(&FANOUT3);
    fan_enable_pwm(&FANOUT4);
    writePinHigh(DIS_PWMIN);
    fan_time = timer_read32();

    return true;
}

bool system76_ec_fan_get_rpm(uint8_t index, uint16_t * rpm) {
    switch (index) {
        case 0:
            *rpm = FANOUT1.tach;
            return true;
        case 1:
            *rpm = FANOUT2.tach;
            return true;
        case 2:
            *rpm = FANOUT3.tach;
            return true;
        case 3:
            *rpm = FANOUT4.tach;
            return true;
        default:
            return false;
    }
}

bool system76_ec_led_get_mode(uint8_t layer, uint8_t * mode, uint8_t * speed) {
    switch (layer) {
        case 0:
            if (driver_suspended) {
                *mode = 1;
            } else {
                *mode = 0;
            }
            *speed = 0;
            return true;
        default:
            return false;
    }
}

bool system76_ec_led_set_mode(uint8_t layer, uint8_t mode, uint8_t speed) {
    switch (layer) {
        case 0:
            if (mode > 0) {
                driver_suspended = true;
            } else {
                driver_suspended = false;
            }
            return true;
        default:
            return false;
    }
}

bool system76_ec_security_get(enum SecurityState * state) {
    if (system76_ec_is_unlocked()) {
        *state = SECURITY_STATE_UNLOCK;
    } else if (readPin(PBRELAY)) {
        *state = SECURITY_STATE_PREPARE_UNLOCK;
    } else {
        *state = SECURITY_STATE_LOCK;
    }
    return true;
}

bool system76_ec_security_set(enum SecurityState state) {
    if (state == SECURITY_STATE_PREPARE_UNLOCK) {
        if (
            // Not already unlocked
            !system76_ec_is_unlocked() &&
            // Not already overriding power button
            !readPin(PBRELAY)
        ) {
            // Set power button override
            unlock_time = timer_read32();
            writePinHigh(PBRELAY);
        }
        return true;
    } else {
        return false;
    }
}
