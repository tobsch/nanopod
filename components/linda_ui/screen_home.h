#pragma once

#include <lvgl.h>
#include <vector>
#include <functional>
#include "../linda_api/api_types.h"

namespace esphome {
namespace linda_ui {

class ScreenHome {
public:
    void create();
    void on_rotate(int direction);
    void on_click();

    lv_obj_t* get_screen() { return screen_; }

    void set_playlists(const std::vector<linda_api::Playlist>& playlists);
    void set_on_select(std::function<void(int)> callback) { on_select_ = callback; }

protected:
    lv_obj_t* screen_ = nullptr;
    lv_obj_t* cover_image_ = nullptr;
    lv_obj_t* title_label_ = nullptr;
    lv_obj_t* indicator_container_ = nullptr;

    std::vector<linda_api::Playlist> playlists_;
    int current_index_ = 0;

    std::function<void(int)> on_select_;

    void update_display();
    void animate_transition(int direction);
};

}  // namespace linda_ui
}  // namespace esphome
