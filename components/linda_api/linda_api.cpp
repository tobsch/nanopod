#include "linda_api.h"
#include "esphome/core/log.h"

namespace esphome {
namespace linda_api {

static const char *TAG = "linda_api";

void LindaAPI::setup() {
    ESP_LOGI(TAG, "Music Assistant API initialized");
    ESP_LOGI(TAG, "Host: %s:%d", host_.c_str(), port_);
    ESP_LOGI(TAG, "Player ID: %s", player_id_.c_str());
}

void LindaAPI::loop() {
    // Poll player state periodically
    unsigned long now = millis();
    if (now - last_state_poll_ >= STATE_POLL_INTERVAL) {
        last_state_poll_ = now;

        if (on_player_state_changed_) {
            PlayerState state = get_player_state();
            on_player_state_changed_(state);
        }
    }
}

std::string LindaAPI::get_base_url() {
    return "http://" + host_ + ":" + std::to_string(port_) + "/api";
}

bool LindaAPI::http_get(const std::string& endpoint, std::string& response) {
    std::string url = get_base_url() + endpoint;

    http_.begin(url.c_str());
    int code = http_.GET();

    if (code == 200) {
        response = http_.getString().c_str();
        http_.end();
        return true;
    }

    ESP_LOGE(TAG, "HTTP GET failed: %s, code: %d", endpoint.c_str(), code);
    http_.end();
    return false;
}

bool LindaAPI::http_post(const std::string& endpoint, const std::string& body) {
    std::string url = get_base_url() + endpoint;

    http_.begin(url.c_str());
    http_.addHeader("Content-Type", "application/json");

    int code = http_.POST(body.c_str());
    http_.end();

    if (code == 200 || code == 204) {
        return true;
    }

    ESP_LOGE(TAG, "HTTP POST failed: %s, code: %d", endpoint.c_str(), code);
    return false;
}

std::vector<Playlist> LindaAPI::get_playlists() {
    std::vector<Playlist> playlists;
    std::string response;

    if (!http_get("/library/playlists", response)) {
        return playlists;
    }

    DynamicJsonDocument doc(8192);
    DeserializationError error = deserializeJson(doc, response);

    if (error) {
        ESP_LOGE(TAG, "JSON parse error: %s", error.c_str());
        return playlists;
    }

    for (JsonObject item : doc.as<JsonArray>()) {
        Playlist p;
        p.id = item["item_id"].as<std::string>();
        p.name = item["name"].as<std::string>();

        if (item.containsKey("image") && !item["image"].isNull()) {
            p.image_url = item["image"]["url"].as<std::string>();
        }

        playlists.push_back(p);
    }

    ESP_LOGI(TAG, "Loaded %d playlists", playlists.size());
    return playlists;
}

std::vector<Track> LindaAPI::get_playlist_tracks(const std::string& playlist_id) {
    std::vector<Track> tracks;
    std::string response;

    std::string endpoint = "/library/playlists/" + playlist_id + "/tracks";
    if (!http_get(endpoint, response)) {
        return tracks;
    }

    DynamicJsonDocument doc(16384);
    DeserializationError error = deserializeJson(doc, response);

    if (error) {
        ESP_LOGE(TAG, "JSON parse error: %s", error.c_str());
        return tracks;
    }

    for (JsonObject item : doc.as<JsonArray>()) {
        Track t;
        t.id = item["item_id"].as<std::string>();
        t.uri = item["uri"].as<std::string>();
        t.name = item["name"].as<std::string>();

        if (item.containsKey("duration")) {
            t.duration_ms = item["duration"].as<int>() * 1000;
        }

        if (item.containsKey("image") && !item["image"].isNull()) {
            t.image_url = item["image"]["url"].as<std::string>();
        }

        tracks.push_back(t);
    }

    ESP_LOGI(TAG, "Loaded %d tracks", tracks.size());
    return tracks;
}

bool LindaAPI::play_track(const std::string& uri) {
    std::string endpoint = "/players/" + player_id_ + "/play_media";
    std::string body = "{\"media\":{\"uri\":\"" + uri + "\"}}";
    return http_post(endpoint, body);
}

bool LindaAPI::pause() {
    std::string endpoint = "/players/" + player_id_ + "/pause";
    return http_post(endpoint);
}

bool LindaAPI::play() {
    std::string endpoint = "/players/" + player_id_ + "/play";
    return http_post(endpoint);
}

bool LindaAPI::stop() {
    std::string endpoint = "/players/" + player_id_ + "/stop";
    return http_post(endpoint);
}

bool LindaAPI::set_volume(int volume) {
    std::string endpoint = "/players/" + player_id_ + "/volume_set";
    std::string body = "{\"volume_level\":" + std::to_string(volume) + "}";
    return http_post(endpoint, body);
}

PlayerState LindaAPI::get_player_state() {
    PlayerState state;
    state.player_id = player_id_;

    std::string response;
    std::string endpoint = "/players/" + player_id_;

    if (!http_get(endpoint, response)) {
        return state;
    }

    DynamicJsonDocument doc(4096);
    DeserializationError error = deserializeJson(doc, response);

    if (error) {
        ESP_LOGE(TAG, "JSON parse error: %s", error.c_str());
        return state;
    }

    state.is_playing = (doc["state"].as<std::string>() == "playing");
    state.volume = doc["volume_level"].as<int>();

    if (doc.containsKey("elapsed_time")) {
        state.position_ms = doc["elapsed_time"].as<int>() * 1000;
    }

    if (doc.containsKey("current_media") && !doc["current_media"].isNull()) {
        JsonObject media = doc["current_media"];
        state.current_track.name = media["name"].as<std::string>();

        if (media.containsKey("duration")) {
            state.duration_ms = media["duration"].as<int>() * 1000;
        }

        if (media.containsKey("image") && !media["image"].isNull()) {
            state.current_track.image_url = media["image"]["url"].as<std::string>();
        }
    }

    return state;
}

void LindaAPI::load_playlists() {
    if (on_playlists_loaded_) {
        std::vector<Playlist> playlists = get_playlists();
        on_playlists_loaded_(playlists);
    }
}

}  // namespace linda_api
}  // namespace esphome
