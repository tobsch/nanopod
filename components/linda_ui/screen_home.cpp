#include "screen_home.h"
#include "ui_theme.h"

namespace esphome {
namespace linda_ui {

void ScreenHome::create() {
    // Create screen
    screen_ = lv_obj_create(nullptr);
    lv_obj_set_style_bg_color(screen_, COLOR_BG_PRIMARY, 0);

    // Cover image container (circular mask)
    lv_obj_t* cover_container = lv_obj_create(screen_);
    lv_obj_set_size(cover_container, COVER_SIZE_LARGE, COVER_SIZE_LARGE);
    lv_obj_align(cover_container, LV_ALIGN_CENTER, 0, -30);
    lv_obj_set_style_radius(cover_container, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_clip_corner(cover_container, true, 0);
    lv_obj_set_style_bg_color(cover_container, COLOR_BG_SECONDARY, 0);
    lv_obj_set_style_border_width(cover_container, 2, 0);
    lv_obj_set_style_border_color(cover_container, COLOR_ACCENT, 0);

    // Cover image (placeholder)
    cover_image_ = lv_img_create(cover_container);
    lv_obj_center(cover_image_);
    // TODO: Set placeholder image

    // Title label
    title_label_ = lv_label_create(screen_);
    lv_obj_set_style_text_font(title_label_, FONT_LARGE, 0);
    lv_obj_set_style_text_color(title_label_, COLOR_TEXT_PRIMARY, 0);
    lv_obj_set_style_text_align(title_label_, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_width(title_label_, 200);
    lv_label_set_long_mode(title_label_, LV_LABEL_LONG_WRAP);
    lv_obj_align(title_label_, LV_ALIGN_CENTER, 0, 60);
    lv_label_set_text(title_label_, "Loading...");

    // Page indicator dots
    indicator_container_ = lv_obj_create(screen_);
    lv_obj_set_size(indicator_container_, 100, 10);
    lv_obj_align(indicator_container_, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_set_style_bg_opa(indicator_container_, LV_OPA_TRANSP, 0);
    lv_obj_set_flex_flow(indicator_container_, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(indicator_container_, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
}

void ScreenHome::on_rotate(int direction) {
    if (playlists_.empty()) return;

    int new_index = current_index_ + direction;

    // Wrap around
    if (new_index < 0) {
        new_index = playlists_.size() - 1;
    } else if (new_index >= static_cast<int>(playlists_.size())) {
        new_index = 0;
    }

    if (new_index != current_index_) {
        animate_transition(direction);
        current_index_ = new_index;
        update_display();
    }
}

void ScreenHome::on_click() {
    if (on_select_ && !playlists_.empty()) {
        on_select_(current_index_);
    }
}

void ScreenHome::set_playlists(const std::vector<linda_api::Playlist>& playlists) {
    playlists_ = playlists;
    current_index_ = 0;

    // Update page indicators
    lv_obj_clean(indicator_container_);
    for (size_t i = 0; i < playlists_.size() && i < 7; i++) {
        lv_obj_t* dot = lv_obj_create(indicator_container_);
        lv_obj_set_size(dot, 8, 8);
        lv_obj_set_style_radius(dot, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_color(dot,
            i == 0 ? COLOR_ACCENT : COLOR_TEXT_SECONDARY, 0);
        lv_obj_set_style_border_width(dot, 0, 0);
    }

    update_display();
}

void ScreenHome::update_display() {
    if (playlists_.empty()) {
        lv_label_set_text(title_label_, "No playlists");
        return;
    }

    const auto& playlist = playlists_[current_index_];
    lv_label_set_text(title_label_, playlist.name.c_str());

    // TODO: Load and display cover image
    // This requires downloading the image and converting to LVGL format

    // Update indicator dots
    int dot_index = 0;
    lv_obj_t* dot = lv_obj_get_child(indicator_container_, dot_index);
    while (dot != nullptr) {
        lv_obj_set_style_bg_color(dot,
            dot_index == current_index_ ? COLOR_ACCENT : COLOR_TEXT_SECONDARY, 0);
        dot_index++;
        dot = lv_obj_get_child(indicator_container_, dot_index);
    }
}

void ScreenHome::animate_transition(int direction) {
    // Simple fade animation for cover change
    lv_anim_t anim;
    lv_anim_init(&anim);
    lv_anim_set_var(&anim, cover_image_);
    lv_anim_set_values(&anim, 255, 100);
    lv_anim_set_time(&anim, ANIM_DURATION_FAST);
    lv_anim_set_exec_cb(&anim, [](void* var, int32_t val) {
        lv_obj_set_style_img_opa(static_cast<lv_obj_t*>(var), val, 0);
    });
    lv_anim_start(&anim);
}

}  // namespace linda_ui
}  // namespace esphome
