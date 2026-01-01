#include "linda_ui.h"
#include "esphome/core/log.h"

namespace esphome {
namespace linda_ui {

static const char *TAG = "linda_ui";

void LindaUI::setup() {
    ESP_LOGI(TAG, "Initializing UI...");

    // Initialize theme
    init_theme();

    // Create screens
    screen_home_ = new ScreenHome();
    screen_series_ = new ScreenSeries();
    screen_player_ = new ScreenPlayer();

    screen_home_->create();
    screen_series_->create();
    screen_player_->create();

    // Set callbacks
    screen_home_->set_on_select([this](int index) {
        selected_playlist_index_ = index;
        // TODO: Load tracks for selected playlist
        navigate_to(Screen::SERIES);
    });

    screen_series_->set_on_select([this](int index) {
        selected_track_index_ = index;
        // TODO: Start playback
        navigate_to(Screen::PLAYER);
    });

    // Show home screen
    lv_scr_load(screen_home_->get_screen());

    ESP_LOGI(TAG, "UI initialized successfully");
}

void LindaUI::loop() {
    // Handle long press timing
    if (button_pressed_) {
        unsigned long press_duration = millis() - button_press_time_;
        if (press_duration >= 2000) {
            on_long_press();
            button_pressed_ = false;
        }
    }
}

void LindaUI::on_rotate(int direction) {
    ESP_LOGD(TAG, "Rotate: %d", direction);

    switch (current_screen_) {
        case Screen::HOME:
            screen_home_->on_rotate(direction);
            break;
        case Screen::SERIES:
            screen_series_->on_rotate(direction);
            break;
        case Screen::PLAYER:
            // Volume control
            screen_player_->on_rotate(direction);
            // TODO: Send volume change to Music Assistant
            break;
    }
}

void LindaUI::on_click() {
    ESP_LOGD(TAG, "Click");

    switch (current_screen_) {
        case Screen::HOME:
            screen_home_->on_click();
            break;
        case Screen::SERIES:
            screen_series_->on_click();
            break;
        case Screen::PLAYER:
            screen_player_->toggle_play_pause();
            // TODO: Send play/pause to Music Assistant
            break;
    }
}

void LindaUI::on_double_click() {
    ESP_LOGD(TAG, "Double click");

    switch (current_screen_) {
        case Screen::HOME:
            // No action
            break;
        case Screen::SERIES:
            navigate_to(Screen::HOME);
            break;
        case Screen::PLAYER:
            navigate_to(Screen::SERIES);
            break;
    }
}

void LindaUI::on_long_press() {
    ESP_LOGD(TAG, "Long press");

    switch (current_screen_) {
        case Screen::PLAYER:
            // Stop and go home
            // TODO: Send stop to Music Assistant
            navigate_to(Screen::HOME);
            break;
        default:
            break;
    }
}

void LindaUI::on_button_press() {
    button_pressed_ = true;
    button_press_time_ = millis();
}

void LindaUI::on_button_release() {
    button_pressed_ = false;
}

void LindaUI::navigate_to(Screen screen) {
    if (screen == current_screen_) return;

    ESP_LOGI(TAG, "Navigate to screen: %d", static_cast<int>(screen));

    lv_obj_t* target_screen = nullptr;
    bool forward = true;

    switch (screen) {
        case Screen::HOME:
            target_screen = screen_home_->get_screen();
            forward = false;
            break;
        case Screen::SERIES:
            target_screen = screen_series_->get_screen();
            forward = (current_screen_ == Screen::HOME);
            break;
        case Screen::PLAYER:
            target_screen = screen_player_->get_screen();
            forward = true;
            break;
    }

    // Animate transition
    if (forward) {
        lv_scr_load_anim(target_screen, LV_SCR_LOAD_ANIM_MOVE_LEFT, ANIM_DURATION_NORMAL, 0, false);
    } else {
        lv_scr_load_anim(target_screen, LV_SCR_LOAD_ANIM_MOVE_RIGHT, ANIM_DURATION_NORMAL, 0, false);
    }

    previous_screen_ = current_screen_;
    current_screen_ = screen;
}

void LindaUI::go_back() {
    navigate_to(previous_screen_);
}

void LindaUI::set_playlists(const std::vector<linda_api::Playlist>& playlists) {
    playlists_ = playlists;
    screen_home_->set_playlists(playlists);
}

void LindaUI::set_tracks(const std::vector<linda_api::Track>& tracks) {
    current_tracks_ = tracks;
    screen_series_->set_tracks(tracks);
}

void LindaUI::update_player_state(const linda_api::PlayerState& state) {
    screen_player_->update_state(state);
}

}  // namespace linda_ui
}  // namespace esphome
