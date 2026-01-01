/**
 * LVGL Configuration for Linda's Audiobook Controller
 * Based on LVGL 8.3.x
 */

#ifndef LV_CONF_H
#define LV_CONF_H

#include <stdint.h>

/*====================
   COLOR SETTINGS
 *====================*/

/* Color depth: 16 (RGB565) */
#define LV_COLOR_DEPTH 16

/* Swap the 2 bytes of RGB565 color for SPI displays */
#define LV_COLOR_16_SWAP 1

/* Enable more complex drawing routines */
#define LV_DRAW_COMPLEX 1

/*====================
   MEMORY SETTINGS
 *====================*/

/* Size of the memory available for `lv_mem_alloc()` in bytes (>= 2kB) */
#define LV_MEM_SIZE (48U * 1024U)

/* Use PSRAM for memory allocation */
#define LV_MEM_CUSTOM 0

/*====================
   HAL SETTINGS
 *====================*/

/* Default display refresh period in milliseconds */
#define LV_DISP_DEF_REFR_PERIOD 16

/* Input device read period in milliseconds */
#define LV_INDEV_DEF_READ_PERIOD 30

/* Use a custom tick source */
#define LV_TICK_CUSTOM 1
#define LV_TICK_CUSTOM_INCLUDE "Arduino.h"
#define LV_TICK_CUSTOM_SYS_TIME_EXPR (millis())

/*====================
   FEATURE CONFIGURATION
 *====================*/

/* Enable animations */
#define LV_USE_ANIMATION 1

/* Enable shadow drawing */
#define LV_USE_SHADOW 1

/* Enable object groups (for keyboard navigation) */
#define LV_USE_GROUP 1

/* Enable GPU interface */
#define LV_USE_GPU 0

/* Enable file system */
#define LV_USE_FILESYSTEM 0

/*====================
   FONT USAGE
 *====================*/

/* Montserrat fonts - enable the ones we need */
#define LV_FONT_MONTSERRAT_8  0
#define LV_FONT_MONTSERRAT_10 0
#define LV_FONT_MONTSERRAT_12 0
#define LV_FONT_MONTSERRAT_14 1
#define LV_FONT_MONTSERRAT_16 0
#define LV_FONT_MONTSERRAT_18 1
#define LV_FONT_MONTSERRAT_20 0
#define LV_FONT_MONTSERRAT_22 0
#define LV_FONT_MONTSERRAT_24 1
#define LV_FONT_MONTSERRAT_26 0
#define LV_FONT_MONTSERRAT_28 0
#define LV_FONT_MONTSERRAT_30 0
#define LV_FONT_MONTSERRAT_32 1
#define LV_FONT_MONTSERRAT_34 0
#define LV_FONT_MONTSERRAT_36 0
#define LV_FONT_MONTSERRAT_38 0
#define LV_FONT_MONTSERRAT_40 0
#define LV_FONT_MONTSERRAT_42 0
#define LV_FONT_MONTSERRAT_44 0
#define LV_FONT_MONTSERRAT_46 0
#define LV_FONT_MONTSERRAT_48 0

/* Enable Montserrat 14 as default font */
#define LV_FONT_DEFAULT &lv_font_montserrat_14

/* Enable symbols */
#define LV_USE_FONT_PLACEHOLDER 1

/* Symbol fonts for play/pause icons */
#define LV_FONT_MONTSERRAT_14_FULL 0
#define LV_USE_FONT_SUBPX 0

/*====================
   WIDGET USAGE
 *====================*/

#define LV_USE_ARC        1
#define LV_USE_BAR        1
#define LV_USE_BTN        1
#define LV_USE_BTNMATRIX  0
#define LV_USE_CANVAS     0
#define LV_USE_CHECKBOX   0
#define LV_USE_DROPDOWN   0
#define LV_USE_IMG        1
#define LV_USE_LABEL      1
#define LV_USE_LINE       1
#define LV_USE_ROLLER     1
#define LV_USE_SLIDER     0
#define LV_USE_SWITCH     0
#define LV_USE_TEXTAREA   0
#define LV_USE_TABLE      0

/* Extra widgets */
#define LV_USE_ANIMIMG    0
#define LV_USE_CALENDAR   0
#define LV_USE_CHART      0
#define LV_USE_COLORWHEEL 0
#define LV_USE_IMGBTN     0
#define LV_USE_KEYBOARD   0
#define LV_USE_LED        0
#define LV_USE_LIST       0
#define LV_USE_MENU       0
#define LV_USE_METER      0
#define LV_USE_MSGBOX     0
#define LV_USE_SPAN       0
#define LV_USE_SPINBOX    0
#define LV_USE_SPINNER    1
#define LV_USE_TABVIEW    0
#define LV_USE_TILEVIEW   0
#define LV_USE_WIN        0

/*====================
   LAYOUTS
 *====================*/

#define LV_USE_FLEX 1
#define LV_USE_GRID 0

/*====================
   LOGGING
 *====================*/

/* Enable logging */
#define LV_USE_LOG 1

#if LV_USE_LOG
    /* Log level: TRACE, INFO, WARN, ERROR, USER, NONE */
    #define LV_LOG_LEVEL LV_LOG_LEVEL_WARN

    /* Print log with printf */
    #define LV_LOG_PRINTF 1
#endif

/*====================
   ASSERTS
 *====================*/

#define LV_USE_ASSERT_NULL          1
#define LV_USE_ASSERT_MALLOC        1
#define LV_USE_ASSERT_STYLE         0
#define LV_USE_ASSERT_MEM_INTEGRITY 0
#define LV_USE_ASSERT_OBJ           0

#endif /* LV_CONF_H */
