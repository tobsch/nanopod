#pragma once

#include <string>
#include <vector>

namespace esphome {
namespace linda_api {

struct Playlist {
    std::string id;
    std::string name;
    std::string image_url;
    int track_count = 0;
};

struct Track {
    std::string id;
    std::string uri;
    std::string name;
    std::string album_name;
    std::string image_url;
    int duration_ms = 0;
};

struct PlayerState {
    std::string player_id;
    std::string player_name;
    bool is_playing = false;
    int volume = 50;
    int position_ms = 0;
    int duration_ms = 0;
    Track current_track;
};

}  // namespace linda_api
}  // namespace esphome
