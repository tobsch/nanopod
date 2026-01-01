#include "ui_theme.h"

namespace esphome {
namespace linda_ui {

static lv_style_t style_panel;
static lv_style_t style_title;
static lv_style_t style_subtitle;
static bool theme_initialized = false;

void init_theme() {
    if (theme_initialized) return;

    // Panel style (rounded containers)
    lv_style_init(&style_panel);
    lv_style_set_bg_color(&style_panel, COLOR_BG_SECONDARY);
    lv_style_set_bg_opa(&style_panel, LV_OPA_COVER);
    lv_style_set_radius(&style_panel, 15);
    lv_style_set_border_width(&style_panel, 0);
    lv_style_set_pad_all(&style_panel, 10);

    // Title text style
    lv_style_init(&style_title);
    lv_style_set_text_font(&style_title, FONT_LARGE);
    lv_style_set_text_color(&style_title, COLOR_TEXT_PRIMARY);
    lv_style_set_text_align(&style_title, LV_TEXT_ALIGN_CENTER);

    // Subtitle text style
    lv_style_init(&style_subtitle);
    lv_style_set_text_font(&style_subtitle, FONT_MEDIUM);
    lv_style_set_text_color(&style_subtitle, COLOR_TEXT_SECONDARY);
    lv_style_set_text_align(&style_subtitle, LV_TEXT_ALIGN_CENTER);

    theme_initialized = true;
}

lv_obj_t* create_rounded_panel(lv_obj_t* parent) {
    lv_obj_t* panel = lv_obj_create(parent);
    lv_obj_add_style(panel, &style_panel, 0);
    return panel;
}

lv_obj_t* create_title_label(lv_obj_t* parent, const char* text) {
    lv_obj_t* label = lv_label_create(parent);
    lv_obj_add_style(label, &style_title, 0);
    lv_label_set_text(label, text);
    return label;
}

lv_obj_t* create_subtitle_label(lv_obj_t* parent, const char* text) {
    lv_obj_t* label = lv_label_create(parent);
    lv_obj_add_style(label, &style_subtitle, 0);
    lv_label_set_text(label, text);
    return label;
}

}  // namespace linda_ui
}  // namespace esphome
