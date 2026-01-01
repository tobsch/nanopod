#include "screen_player.h"
#include "ui_theme.h"

namespace esphome {
namespace linda_ui {

void ScreenPlayer::create() {
    screen_ = lv_obj_create(nullptr);
    lv_obj_set_style_bg_color(screen_, COLOR_BG_PRIMARY, 0);

    // Volume arc (outer ring)
    volume_arc_ = lv_arc_create(screen_);
    lv_obj_set_size(volume_arc_, 230, 230);
    lv_obj_center(volume_arc_);
    lv_arc_set_rotation(volume_arc_, 135);
    lv_arc_set_bg_angles(volume_arc_, 0, 270);
    lv_arc_set_range(volume_arc_, 0, 100);
    lv_arc_set_value(volume_arc_, volume_);
    lv_obj_set_style_arc_width(volume_arc_, 8, LV_PART_MAIN);
    lv_obj_set_style_arc_color(volume_arc_, COLOR_BG_SECONDARY, LV_PART_MAIN);
    lv_obj_set_style_arc_width(volume_arc_, 8, LV_PART_INDICATOR);
    lv_obj_set_style_arc_color(volume_arc_, COLOR_ACCENT, LV_PART_INDICATOR);
    lv_obj_remove_style(volume_arc_, nullptr, LV_PART_KNOB);  // Hide knob

    // Cover container
    cover_container_ = lv_obj_create(screen_);
    lv_obj_set_size(cover_container_, COVER_SIZE_MEDIUM, COVER_SIZE_MEDIUM);
    lv_obj_align(cover_container_, LV_ALIGN_CENTER, 0, -25);
    lv_obj_set_style_radius(cover_container_, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_clip_corner(cover_container_, true, 0);
    lv_obj_set_style_bg_color(cover_container_, COLOR_BG_SECONDARY, 0);
    lv_obj_set_style_border_width(cover_container_, 0, 0);

    // Play/pause icon overlay
    play_icon_ = lv_label_create(cover_container_);
    lv_obj_set_style_text_font(play_icon_, FONT_XLARGE, 0);
    lv_obj_set_style_text_color(play_icon_, COLOR_TEXT_PRIMARY, 0);
    lv_obj_center(play_icon_);
    lv_label_set_text(play_icon_, LV_SYMBOL_PAUSE);

    // Title
    title_label_ = lv_label_create(screen_);
    lv_obj_set_style_text_font(title_label_, FONT_MEDIUM, 0);
    lv_obj_set_style_text_color(title_label_, COLOR_TEXT_PRIMARY, 0);
    lv_obj_set_style_text_align(title_label_, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_width(title_label_, 180);
    lv_label_set_long_mode(title_label_, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_align(title_label_, LV_ALIGN_CENTER, 0, 55);
    lv_label_set_text(title_label_, "No track");

    // Progress bar
    progress_bar_ = lv_bar_create(screen_);
    lv_obj_set_size(progress_bar_, 160, 6);
    lv_obj_align(progress_bar_, LV_ALIGN_CENTER, 0, 80);
    lv_bar_set_range(progress_bar_, 0, 100);
    lv_bar_set_value(progress_bar_, 0, LV_ANIM_OFF);
    lv_obj_set_style_bg_color(progress_bar_, COLOR_BG_SECONDARY, LV_PART_MAIN);
    lv_obj_set_style_bg_color(progress_bar_, COLOR_ACCENT, LV_PART_INDICATOR);
    lv_obj_set_style_radius(progress_bar_, 3, 0);

    // Time label
    time_label_ = lv_label_create(screen_);
    lv_obj_set_style_text_font(time_label_, FONT_SMALL, 0);
    lv_obj_set_style_text_color(time_label_, COLOR_TEXT_SECONDARY, 0);
    lv_obj_align(time_label_, LV_ALIGN_CENTER, 0, 95);
    lv_label_set_text(time_label_, "00:00");
}

void ScreenPlayer::on_rotate(int direction) {
    volume_ += direction * 5;

    if (volume_ < 0) volume_ = 0;
    if (volume_ > 100) volume_ = 100;

    update_volume_display();
}

void ScreenPlayer::toggle_play_pause() {
    is_playing_ = !is_playing_;
    update_play_icon();
}

void ScreenPlayer::update_state(const linda_api::PlayerState& state) {
    is_playing_ = state.is_playing;
    volume_ = state.volume;

    lv_label_set_text(title_label_, state.current_track.name.c_str());

    // Update progress
    if (state.duration_ms > 0) {
        int progress = (state.position_ms * 100) / state.duration_ms;
        lv_bar_set_value(progress_bar_, progress, LV_ANIM_ON);
    }

    // Update time
    int seconds = state.position_ms / 1000;
    int minutes = seconds / 60;
    seconds = seconds % 60;
    char time_str[16];
    snprintf(time_str, sizeof(time_str), "%02d:%02d", minutes, seconds);
    lv_label_set_text(time_label_, time_str);

    update_volume_display();
    update_play_icon();
}

void ScreenPlayer::update_volume_display() {
    lv_arc_set_value(volume_arc_, volume_);
}

void ScreenPlayer::update_play_icon() {
    lv_label_set_text(play_icon_, is_playing_ ? LV_SYMBOL_PAUSE : LV_SYMBOL_PLAY);
}

}  // namespace linda_ui
}  // namespace esphome
