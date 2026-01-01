#pragma once

#include <lvgl.h>
#include <vector>
#include <functional>
#include "../linda_api/api_types.h"

namespace esphome {
namespace linda_ui {

class ScreenSeries {
public:
    void create();
    void on_rotate(int direction);
    void on_click();

    lv_obj_t* get_screen() { return screen_; }

    void set_tracks(const std::vector<linda_api::Track>& tracks);
    void set_title(const std::string& title);
    void set_on_select(std::function<void(int)> callback) { on_select_ = callback; }

protected:
    lv_obj_t* screen_ = nullptr;
    lv_obj_t* header_label_ = nullptr;
    lv_obj_t* list_ = nullptr;

    std::vector<linda_api::Track> tracks_;
    int current_index_ = 0;

    std::function<void(int)> on_select_;

    void update_selection();
};

}  // namespace linda_ui
}  // namespace esphome
