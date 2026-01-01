#pragma once

#include "esphome/core/component.h"
#include "api_types.h"
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <vector>
#include <functional>

namespace esphome {
namespace linda_api {

class LindaAPI : public Component {
public:
    void set_host(const std::string& host) { host_ = host; }
    void set_port(uint16_t port) { port_ = port; }
    void set_player_id(const std::string& player_id) { player_id_ = player_id; }

    void setup() override;
    void loop() override;
    float get_setup_priority() const override { return setup_priority::AFTER_WIFI; }

    // API methods
    std::vector<Playlist> get_playlists();
    std::vector<Track> get_playlist_tracks(const std::string& playlist_id);
    bool play_track(const std::string& uri);
    bool pause();
    bool play();
    bool stop();
    bool set_volume(int volume);
    PlayerState get_player_state();

    // Callbacks
    void set_on_playlists_loaded(std::function<void(const std::vector<Playlist>&)> cb) {
        on_playlists_loaded_ = cb;
    }
    void set_on_player_state_changed(std::function<void(const PlayerState&)> cb) {
        on_player_state_changed_ = cb;
    }

    // Trigger async loads
    void load_playlists();
    void load_playlist_tracks(const std::string& playlist_id);

protected:
    std::string host_;
    uint16_t port_ = 8095;
    std::string player_id_;

    HTTPClient http_;
    unsigned long last_state_poll_ = 0;
    static const unsigned long STATE_POLL_INTERVAL = 1000;

    std::function<void(const std::vector<Playlist>&)> on_playlists_loaded_;
    std::function<void(const PlayerState&)> on_player_state_changed_;

    std::string get_base_url();
    bool http_get(const std::string& endpoint, std::string& response);
    bool http_post(const std::string& endpoint, const std::string& body = "");
};

}  // namespace linda_api
}  // namespace esphome
