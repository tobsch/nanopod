#pragma once

#include <lvgl.h>

namespace esphome {
namespace linda_ui {

// Color scheme
#define COLOR_BG_PRIMARY      lv_color_hex(0x1a1a2e)
#define COLOR_BG_SECONDARY    lv_color_hex(0x16213e)
#define COLOR_ACCENT          lv_color_hex(0x1DB954)  // Spotify green
#define COLOR_TEXT_PRIMARY    lv_color_hex(0xFFFFFF)
#define COLOR_TEXT_SECONDARY  lv_color_hex(0xB3B3B3)
#define COLOR_HIGHLIGHT       lv_color_hex(0xFFD700)
#define COLOR_ERROR           lv_color_hex(0xFF4444)

// Dimensions
#define SCREEN_WIDTH          240
#define SCREEN_HEIGHT         240
#define SCREEN_CENTER_X       120
#define SCREEN_CENTER_Y       120

// Cover image sizes
#define COVER_SIZE_LARGE      120
#define COVER_SIZE_MEDIUM     100
#define COVER_SIZE_SMALL      60

// Animation durations (ms)
#define ANIM_DURATION_FAST    100
#define ANIM_DURATION_NORMAL  150
#define ANIM_DURATION_SLOW    300

// Font sizes (use built-in LVGL Montserrat)
#define FONT_SMALL            &lv_font_montserrat_14
#define FONT_MEDIUM           &lv_font_montserrat_18
#define FONT_LARGE            &lv_font_montserrat_24
#define FONT_XLARGE           &lv_font_montserrat_32

// Initialize theme
void init_theme();

// Create styled objects
lv_obj_t* create_rounded_panel(lv_obj_t* parent);
lv_obj_t* create_title_label(lv_obj_t* parent, const char* text);
lv_obj_t* create_subtitle_label(lv_obj_t* parent, const char* text);

}  // namespace linda_ui
}  // namespace esphome
