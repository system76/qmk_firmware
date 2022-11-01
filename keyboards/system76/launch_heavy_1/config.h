#ifndef CONFIG_H
#define CONFIG_H

#include "config_common.h"

/* USB Device descriptor parameter */
#define VENDOR_ID       0x3384
#define PRODUCT_ID      0x0007
#define DEVICE_VER      0x0001
#define MANUFACTURER    System76
#define PRODUCT         Launch Heavy Configurable Keyboard (launch_heavy_1)
#define DESCRIPTION     Launch Heavy Configurable Keyboard (launch_heavy_1)

/* key matrix size */
#define MATRIX_ROWS 12
#define MATRIX_COLS 14

/* key matrix pins */
#define MATRIX_ROW_PINS { F0, F1, F2, F3, F4, F5, D2, D4, D5, D6, D7, E0 }
#define MATRIX_COL_PINS { C6, C5, C4, C3, C2, C1, C7, A7, A6, A5, A4, E6, C0, E7 }
#define UNUSED_PINS

/* COL2ROW or ROW2COL */
#define DIODE_DIRECTION COL2ROW

/* Set 0 if debouncing isn't needed */
#define DEBOUNCE 5

// NKRO must be used
#define FORCE_NKRO

#if RGBLIGHT_ENABLE
    #define RGB_DI_PIN F7
    #define RGBLED_NUM 70
    #define RGBLIGHT_ANIMATIONS
    // Limit brightness to support USB-A at 0.5A
    //TODO: do this dynamically based on power source
    #define RGBLIGHT_LIMIT_VAL 176
#endif

#if RGB_MATRIX_ENABLE
    #define RGB_DI_PIN F7
    #define DRIVER_LED_TOTAL 105
    #define RGB_MATRIX_KEYPRESSES // reacts to keypresses
    //#define RGB_MATRIX_KEYRELEASES // reacts to keyreleases (instead of keypresses)
    //#define RGB_MATRIX_FRAMEBUFFER_EFFECTS
    #define RGB_DISABLE_TIMEOUT 0 // number of milliseconds to wait until rgb automatically turns off
    #define RGB_DISABLE_AFTER_TIMEOUT 0 // OBSOLETE: number of ticks to wait until disabling effects
    #define RGB_DISABLE_WHEN_USB_SUSPENDED true // turn off effects when suspended
    #define RGB_MATRIX_MAXIMUM_BRIGHTNESS 176 // limits maximum brightness of LEDs to 200 out of 255. If not defined maximum brightness is set to 255
    #define RGB_MATRIX_STARTUP_MODE RGB_MATRIX_RAINBOW_MOVING_CHEVRON // Sets the default mode, if none has been set
    #define RGB_MATRIX_STARTUP_HUE 142 // 200 degrees
    #define RGB_MATRIX_STARTUP_SAT 255
    #define RGB_MATRIX_STARTUP_VAL RGB_MATRIX_MAXIMUM_BRIGHTNESS
    #define RGB_MATRIX_STARTUP_SPD 127
    #define RGB_MATRIX_DISABLE_KEYCODES // disables control of rgb matrix by keycodes (must use code functions to control the feature)
#endif

/* Mechanical locking support. Use KC_LCAP, KC_LNUM or KC_LSCR instead in keymap */
#define LOCKING_SUPPORT_ENABLE

/* Locking resynchronize hack */
#define LOCKING_RESYNC_ENABLE

// EEPROM {
#define EEPROM_SIZE 2048

// TODO: refactor with new user EEPROM code (coming soon)
#define EEPROM_MAGIC 0x76EC
#define EEPROM_MAGIC_ADDR 64
// Bump this every time we change what we store
// This will automatically reset the EEPROM with defaults
// and avoid loading invalid data from the EEPROM
#define EEPROM_VERSION 0x02
#define EEPROM_VERSION_ADDR (EEPROM_MAGIC_ADDR + 2)
// } EEPROM

// Dynamic keyboard support {
#define DYNAMIC_KEYMAP_LAYER_COUNT 4
// Dynamic keymap starts after EEPROM version
#define DYNAMIC_KEYMAP_EEPROM_ADDR (EEPROM_VERSION_ADDR + 1)
// `* 2` Beacuse each key needs 16 bits (2 bytes) for storing the keycode
#define DYNAMIC_KEYMAP_EEPROM_SIZE (DYNAMIC_KEYMAP_LAYER_COUNT * MATRIX_ROWS * MATRIX_COLS * 2)
// Dynamic macro starts after dynamic keymaps, it is disabled
#define DYNAMIC_KEYMAP_MACRO_EEPROM_ADDR (DYNAMIC_KEYMAP_EEPROM_ADDR + DYNAMIC_KEYMAP_EEPROM_SIZE)
#define DYNAMIC_KEYMAP_MACRO_EEPROM_SIZE 0
#define DYNAMIC_KEYMAP_MACRO_COUNT 0
// } Dynamic keyboard support

// System76 EC {
#define SYSTEM76_EC_EEPROM_ADDR (DYNAMIC_KEYMAP_MACRO_EEPROM_ADDR + DYNAMIC_KEYMAP_MACRO_EEPROM_SIZE)
#define SYSTEM76_EC_EEPROM_SIZE (EEPROM_SIZE - SYSTEM76_EC_EEPROM_ADDR)
// } System76 EC

#endif // CONFIG_H
