#pragma once

#include "esphome/core/component.h"
#include "ui_theme.h"
#include "screen_home.h"
#include "screen_series.h"
#include "screen_player.h"
#include "../linda_api/linda_api.h"

#include <lvgl.h>
#include <vector>
#include <string>

namespace esphome {
namespace linda_ui {

enum class Screen {
    HOME,
    SERIES,
    PLAYER
};

enum class InputEvent {
    ROTATE_CW,
    ROTATE_CCW,
    CLICK,
    DOUBLE_CLICK,
    LONG_PRESS
};

class LindaUI : public Component {
public:
    void setup() override;
    void loop() override;
    float get_setup_priority() const override { return setup_priority::PROCESSOR - 1; }

    // Input handlers (called from YAML lambdas)
    void on_rotate(int direction);
    void on_click();
    void on_double_click();
    void on_long_press();
    void on_button_press();
    void on_button_release();

    // Navigation
    void navigate_to(Screen screen);
    void go_back();

    // Data updates
    void set_playlists(const std::vector<linda_api::Playlist>& playlists);
    void set_tracks(const std::vector<linda_api::Track>& tracks);
    void update_player_state(const linda_api::PlayerState& state);

    // LED feedback
    void set_led_mode(int mode, int value = 0);

protected:
    Screen current_screen_ = Screen::HOME;
    Screen previous_screen_ = Screen::HOME;

    // Screen instances
    ScreenHome* screen_home_ = nullptr;
    ScreenSeries* screen_series_ = nullptr;
    ScreenPlayer* screen_player_ = nullptr;

    // Current data
    std::vector<linda_api::Playlist> playlists_;
    std::vector<linda_api::Track> current_tracks_;
    int selected_playlist_index_ = 0;
    int selected_track_index_ = 0;

    // Button state for long press detection
    unsigned long button_press_time_ = 0;
    bool button_pressed_ = false;

    // Screen transition animation
    void animate_screen_transition(lv_obj_t* from, lv_obj_t* to, bool forward);
};

}  // namespace linda_ui
}  // namespace esphome
