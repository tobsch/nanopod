#include "screen_series.h"
#include "ui_theme.h"

namespace esphome {
namespace linda_ui {

void ScreenSeries::create() {
    screen_ = lv_obj_create(nullptr);
    lv_obj_set_style_bg_color(screen_, COLOR_BG_PRIMARY, 0);

    // Header
    header_label_ = lv_label_create(screen_);
    lv_obj_set_style_text_font(header_label_, FONT_MEDIUM, 0);
    lv_obj_set_style_text_color(header_label_, COLOR_ACCENT, 0);
    lv_obj_align(header_label_, LV_ALIGN_TOP_MID, 0, 15);
    lv_label_set_text(header_label_, "Episodes");

    // Divider line
    lv_obj_t* divider = lv_obj_create(screen_);
    lv_obj_set_size(divider, 180, 2);
    lv_obj_align(divider, LV_ALIGN_TOP_MID, 0, 45);
    lv_obj_set_style_bg_color(divider, COLOR_TEXT_SECONDARY, 0);
    lv_obj_set_style_border_width(divider, 0, 0);

    // Track list (using roller for smooth scrolling)
    list_ = lv_roller_create(screen_);
    lv_obj_set_size(list_, 200, 140);
    lv_obj_align(list_, LV_ALIGN_CENTER, 0, 20);
    lv_obj_set_style_bg_color(list_, COLOR_BG_PRIMARY, 0);
    lv_obj_set_style_bg_color(list_, COLOR_BG_SECONDARY, LV_PART_SELECTED);
    lv_obj_set_style_text_color(list_, COLOR_TEXT_SECONDARY, 0);
    lv_obj_set_style_text_color(list_, COLOR_TEXT_PRIMARY, LV_PART_SELECTED);
    lv_obj_set_style_text_font(list_, FONT_MEDIUM, 0);
    lv_obj_set_style_border_width(list_, 0, 0);
    lv_roller_set_visible_row_count(list_, 4);
}

void ScreenSeries::on_rotate(int direction) {
    if (tracks_.empty()) return;

    int new_index = current_index_ + direction;

    if (new_index >= 0 && new_index < static_cast<int>(tracks_.size())) {
        current_index_ = new_index;
        lv_roller_set_selected(list_, current_index_, LV_ANIM_ON);
    }
}

void ScreenSeries::on_click() {
    if (on_select_ && !tracks_.empty()) {
        on_select_(current_index_);
    }
}

void ScreenSeries::set_tracks(const std::vector<linda_api::Track>& tracks) {
    tracks_ = tracks;
    current_index_ = 0;

    // Build options string for roller
    std::string options;
    for (size_t i = 0; i < tracks_.size(); i++) {
        if (i > 0) options += "\n";
        options += tracks_[i].name;
    }

    lv_roller_set_options(list_, options.c_str(), LV_ROLLER_MODE_NORMAL);
    lv_roller_set_selected(list_, 0, LV_ANIM_OFF);
}

void ScreenSeries::set_title(const std::string& title) {
    lv_label_set_text(header_label_, title.c_str());
}

}  // namespace linda_ui
}  // namespace esphome
