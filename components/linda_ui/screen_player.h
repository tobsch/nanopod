#pragma once

#include <lvgl.h>
#include "../linda_api/api_types.h"

namespace esphome {
namespace linda_ui {

class ScreenPlayer {
public:
    void create();
    void on_rotate(int direction);
    void toggle_play_pause();

    lv_obj_t* get_screen() { return screen_; }

    void update_state(const linda_api::PlayerState& state);
    int get_volume() { return volume_; }

protected:
    lv_obj_t* screen_ = nullptr;
    lv_obj_t* volume_arc_ = nullptr;
    lv_obj_t* cover_container_ = nullptr;
    lv_obj_t* cover_image_ = nullptr;
    lv_obj_t* play_icon_ = nullptr;
    lv_obj_t* title_label_ = nullptr;
    lv_obj_t* progress_bar_ = nullptr;
    lv_obj_t* time_label_ = nullptr;

    int volume_ = 50;
    bool is_playing_ = false;

    void update_volume_display();
    void update_play_icon();
};

}  // namespace linda_ui
}  // namespace esphome
