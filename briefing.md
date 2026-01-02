# NanoPod

## Project Overview

An iPod-inspired audiobook controller for children. Simple audiobook playback on a room speaker – without a phone, without complicated apps.

### Goals
- Simple, child-friendly operation
- Turn + Click like an iPod
- Dynamic playlists from Music Assistant / Spotify
- Playback on room speaker

### Technology Decision
- **Framework:** ESPHome (YAML + C++ Custom Components)
- **UI Library:** LVGL 8.x (integrated via custom component)
- **Benefits:** OTA updates, Home Assistant integration, WiFi/API via YAML

---

## Hardware

### CrowPanel 1.28" HMI ESP32 Rotary Display

**Manufacturer:** Elecrow  
**Documentation:** https://www.elecrow.com/wiki/CrowPanel_1.28inch-HMI_ESP32_Rotary_Display.html  
**GitHub:** https://github.com/Elecrow-RD/CrowPanel-1.28inch-HMI-ESP32-Rotary-Display-240-240-IPS-Round-Touch-Knob-Screen

#### Specifications

| Component | Details |
|-----------|---------|
| **MCU** | ESP32-S3R8, Dual-Core 240MHz |
| **RAM** | 512KB SRAM + 8MB PSRAM |
| **Flash** | 16MB |
| **Display** | 1.28" IPS, 240x240px, round |
| **Display Driver** | GC9A01 (SPI) |
| **Touch** | CST816D (I2C), capacitive |
| **WiFi** | 802.11 b/g/n, 2.4GHz |
| **Bluetooth** | BLE 5.0 |
| **RGB LEDs** | 5x WS2812B |

#### Pin Mapping

```cpp
// Display (GC9A01 - SPI)
#define TFT_SCLK    10
#define TFT_MOSI    11
#define TFT_DC      3
#define TFT_CS      9
#define TFT_RST     14
#define TFT_BL      46    // Backlight PWM

// Touch (CST816D - I2C)
#define TP_SDA      6
#define TP_SCL      7
#define TP_INT      5
#define TP_RST      13

// Rotary Encoder
#define ENCODER_A   45
#define ENCODER_B   42
#define ENCODER_SW  41    // Push-Button

// RGB LEDs (WS2812B)
#define LED_PIN     48
#define LED_COUNT   5

// I2C Bus (optional expansion)
#define I2C_SDA     38
#define I2C_SCL     39

// Misc
#define POWER_LED   40
```

---

## Software Architecture

### ESPHome + Custom Components Approach

```
┌─────────────────────────────────────────────────────────────┐
│                     ESPHome YAML                            │
│   (WiFi, API, OTA, Sensors, Basic Config)                   │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                  Custom Components (C++)                    │
│                                                             │
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────┐ │
│  │  linda_display  │  │  linda_ui       │  │ linda_api   │ │
│  │  ─────────────  │  │  ────────       │  │ ─────────   │ │
│  │  - GC9A01 init  │  │  - LVGL screens │  │ - MA client │ │
│  │  - LVGL setup   │  │  - Navigation   │  │ - Playlists │ │
│  │  - Backlight    │  │  - Animations   │  │ - Playback  │ │
│  └─────────────────┘  └─────────────────┘  └─────────────┘ │
│                                                             │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│                    External Libraries                       │
│                                                             │
│   LVGL 8.x    │    LovyanGFX    │    ArduinoJson           │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### Project File Structure

```
nanopod/
│
├── nanopod.yaml                    # Main ESPHome config
├── secrets.yaml                    # WiFi, API keys
│
├── components/
│   │
│   ├── linda_display/              # Display & LVGL initialization
│   │   ├── __init__.py             # ESPHome component registration
│   │   ├── linda_display.h         # Header
│   │   └── linda_display.cpp       # Implementation
│   │
│   ├── linda_ui/                   # UI screens & navigation
│   │   ├── __init__.py
│   │   ├── linda_ui.h
│   │   ├── linda_ui.cpp
│   │   ├── screen_home.h           # Home screen (series carousel)
│   │   ├── screen_home.cpp
│   │   ├── screen_series.h         # Series screen (episode list)
│   │   ├── screen_series.cpp
│   │   ├── screen_player.h         # Now playing screen
│   │   ├── screen_player.cpp
│   │   └── ui_theme.h              # Colors, fonts, styles
│   │
│   └── linda_api/                  # Music Assistant client
│       ├── __init__.py
│       ├── linda_api.h
│       ├── linda_api.cpp
│       └── api_types.h             # Data structures
│
└── fonts/                          # Custom fonts (if needed)
    └── montserrat_24.c
```

---

## ESPHome Configuration

### Main YAML: nanopod.yaml

```yaml
esphome:
  name: nanopod
  friendly_name: "NanoPod"
  platformio_options:
    board_build.flash_mode: qio
    board_build.psram_type: opi

esp32:
  board: esp32-s3-devkitc-1
  variant: esp32s3
  framework:
    type: arduino
    version: recommended
  flash_size: 16MB

psram:
  mode: octal
  speed: 80MHz

# External components (local)
external_components:
  - source:
      type: local
      path: components
    components: [linda_display, linda_ui, linda_api]

# WiFi
wifi:
  ssid: !secret wifi_ssid
  password: !secret wifi_password
  fast_connect: true
  
  ap:
    ssid: "NanoPod-AP"
    password: !secret ap_password

captive_portal:

# Home Assistant API
api:
  encryption:
    key: !secret api_key

# OTA Updates
ota:
  - platform: esphome
    password: !secret ota_password

# Logging
logger:
  level: DEBUG

# ============================================
# Hardware Configuration
# ============================================

# SPI Bus for Display
spi:
  clk_pin: GPIO10
  mosi_pin: GPIO11

# I2C Bus for Touch
i2c:
  - id: bus_touch
    sda: GPIO6
    scl: GPIO7
    scan: false

# Rotary Encoder
sensor:
  - platform: rotary_encoder
    id: dial
    pin_a: GPIO45
    pin_b: GPIO42
    resolution: 1
    on_clockwise:
      - lambda: |-
          linda_ui->on_rotate(1);
    on_anticlockwise:
      - lambda: |-
          linda_ui->on_rotate(-1);

# Encoder Button
binary_sensor:
  - platform: gpio
    id: dial_button
    pin:
      number: GPIO41
      mode: INPUT_PULLUP
      inverted: true
    filters:
      - delayed_on: 20ms
      - delayed_off: 20ms
    on_press:
      - lambda: |-
          linda_ui->on_button_press();
    on_release:
      - lambda: |-
          linda_ui->on_button_release();
    on_click:
      min_length: 0ms
      max_length: 500ms
      then:
        - lambda: |-
            linda_ui->on_click();
    on_double_click:
      min_length: 0ms
      max_length: 300ms
      then:
        - lambda: |-
            linda_ui->on_double_click();
    on_multi_click:
      - timing:
          - ON for at least 2s
        then:
          - lambda: |-
              linda_ui->on_long_press();

# RGB LEDs
light:
  - platform: neopixelbus
    id: rgb_leds
    type: GRB
    variant: WS2812
    pin: GPIO48
    num_leds: 5
    name: "Status LEDs"

# Display Backlight
output:
  - platform: ledc
    id: backlight_pwm
    pin: GPIO46
    frequency: 5000Hz

light:
  - platform: monochromatic
    id: backlight
    output: backlight_pwm
    name: "Display Backlight"
    default_transition_length: 200ms

# ============================================
# Custom Components
# ============================================

# Display Component
linda_display:
  id: main_display
  cs_pin: GPIO9
  dc_pin: GPIO3
  reset_pin: GPIO14
  backlight: backlight

# UI Component
linda_ui:
  id: linda_ui
  display_id: main_display
  api_id: music_api
  leds_id: rgb_leds

# Music Assistant API
linda_api:
  id: music_api
  host: !secret ma_host
  port: 8095
  player_id: !secret ma_player_id
```

### secrets.yaml

```yaml
wifi_ssid: "YOUR_WIFI_SSID"
wifi_password: "YOUR_WIFI_PASSWORD"
ap_password: "fallback123"
api_key: "YOUR_HA_API_KEY"
ota_password: "YOUR_OTA_PASSWORD"

ma_host: "homeassistant.local"
ma_player_id: "lindas_zimmer"
```

---

## Custom Components

### 1. linda_display Component

#### components/linda_display/__init__.py

```python
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import spi
from esphome.const import CONF_ID, CONF_CS_PIN, CONF_DC_PIN, CONF_RESET_PIN

DEPENDENCIES = ["spi"]
AUTO_LOAD = ["spi"]

CONF_BACKLIGHT = "backlight"

linda_display_ns = cg.esphome_ns.namespace("linda_display")
LindaDisplay = linda_display_ns.class_("LindaDisplay", cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(LindaDisplay),
    cv.Required(CONF_CS_PIN): cv.int_,
    cv.Required(CONF_DC_PIN): cv.int_,
    cv.Required(CONF_RESET_PIN): cv.int_,
    cv.Optional(CONF_BACKLIGHT): cv.use_id(cg.EntityBase),
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    
    cg.add(var.set_pins(
        config[CONF_CS_PIN],
        config[CONF_DC_PIN],
        config[CONF_RESET_PIN]
    ))
    
    cg.add_library("lvgl/lvgl", "8.3.11")
    cg.add_library("lovyan03/LovyanGFX", "1.1.12")
    
    cg.add_build_flag("-DLV_CONF_INCLUDE_SIMPLE")
    cg.add_build_flag("-DLV_LVGL_H_INCLUDE_SIMPLE")
```

#### components/linda_display/linda_display.h

```cpp
#pragma once

#include "esphome/core/component.h"
#include "esphome/core/log.h"

#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <lvgl.h>

namespace esphome {
namespace linda_display {

// Custom LGFX class for GC9A01
class LGFX_LindaDisplay : public lgfx::LGFX_Device {
public:
    lgfx::Panel_GC9A01 _panel_instance;
    lgfx::Bus_SPI _bus_instance;

    LGFX_LindaDisplay() {
        // SPI bus configuration
        {
            auto cfg = _bus_instance.config();
            cfg.spi_host = SPI2_HOST;
            cfg.spi_mode = 0;
            cfg.freq_write = 80000000;
            cfg.freq_read = 20000000;
            cfg.spi_3wire = true;
            cfg.use_lock = true;
            cfg.dma_channel = SPI_DMA_CH_AUTO;
            cfg.pin_sclk = 10;
            cfg.pin_mosi = 11;
            cfg.pin_miso = -1;
            cfg.pin_dc = 3;
            _bus_instance.config(cfg);
            _panel_instance.setBus(&_bus_instance);
        }

        // Panel configuration
        {
            auto cfg = _panel_instance.config();
            cfg.pin_cs = 9;
            cfg.pin_rst = 14;
            cfg.pin_busy = -1;
            cfg.memory_width = 240;
            cfg.memory_height = 240;
            cfg.panel_width = 240;
            cfg.panel_height = 240;
            cfg.offset_x = 0;
            cfg.offset_y = 0;
            cfg.offset_rotation = 0;
            cfg.dummy_read_pixel = 8;
            cfg.dummy_read_bits = 1;
            cfg.readable = false;
            cfg.invert = true;
            cfg.rgb_order = false;
            cfg.dlen_16bit = false;
            cfg.bus_shared = false;
            _panel_instance.config(cfg);
        }

        setPanel(&_panel_instance);
    }
};

class LindaDisplay : public Component {
public:
    void set_pins(int cs, int dc, int rst) {
        cs_pin_ = cs;
        dc_pin_ = dc;
        rst_pin_ = rst;
    }

    void setup() override;
    void loop() override;
    float get_setup_priority() const override { return setup_priority::PROCESSOR; }

    LGFX_LindaDisplay* get_gfx() { return &gfx_; }
    
    static const uint16_t SCREEN_WIDTH = 240;
    static const uint16_t SCREEN_HEIGHT = 240;

protected:
    LGFX_LindaDisplay gfx_;
    int cs_pin_;
    int dc_pin_;
    int rst_pin_;

    // LVGL display buffer
    static lv_disp_draw_buf_t draw_buf_;
    static lv_color_t buf1_[SCREEN_WIDTH * 20];
    static lv_color_t buf2_[SCREEN_WIDTH * 20];
    static lv_disp_drv_t disp_drv_;

    static void lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_p);
    static LGFX_LindaDisplay* gfx_instance_;
};

}  // namespace linda_display
}  // namespace esphome
```

#### components/linda_display/linda_display.cpp

```cpp
#include "linda_display.h"

namespace esphome {
namespace linda_display {

static const char *TAG = "linda_display";

// Static member initialization
lv_disp_draw_buf_t LindaDisplay::draw_buf_;
lv_color_t LindaDisplay::buf1_[SCREEN_WIDTH * 20];
lv_color_t LindaDisplay::buf2_[SCREEN_WIDTH * 20];
lv_disp_drv_t LindaDisplay::disp_drv_;
LGFX_LindaDisplay* LindaDisplay::gfx_instance_ = nullptr;

void LindaDisplay::setup() {
    ESP_LOGI(TAG, "Initializing display...");

    // Initialize display
    gfx_.init();
    gfx_.setRotation(0);
    gfx_.setBrightness(255);
    gfx_.fillScreen(TFT_BLACK);
    
    gfx_instance_ = &gfx_;

    // Initialize LVGL
    lv_init();

    // Initialize display buffer
    lv_disp_draw_buf_init(&draw_buf_, buf1_, buf2_, SCREEN_WIDTH * 20);

    // Initialize display driver
    lv_disp_drv_init(&disp_drv_);
    disp_drv_.hor_res = SCREEN_WIDTH;
    disp_drv_.ver_res = SCREEN_HEIGHT;
    disp_drv_.flush_cb = lvgl_flush_cb;
    disp_drv_.draw_buf = &draw_buf_;
    lv_disp_drv_register(&disp_drv_);

    ESP_LOGI(TAG, "Display initialized successfully");
}

void LindaDisplay::loop() {
    lv_timer_handler();
}

void LindaDisplay::lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_p) {
    if (gfx_instance_ == nullptr) return;

    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    gfx_instance_->startWrite();
    gfx_instance_->setAddrWindow(area->x1, area->y1, w, h);
    gfx_instance_->writePixels((lgfx::rgb565_t *)&color_p->full, w * h);
    gfx_instance_->endWrite();

    lv_disp_flush_ready(drv);
}

}  // namespace linda_display
}  // namespace esphome
```

### 2. linda_ui Component

#### components/linda_ui/__init__.py

```python
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID

DEPENDENCIES = ["linda_display", "linda_api"]

CONF_DISPLAY_ID = "display_id"
CONF_API_ID = "api_id"
CONF_LEDS_ID = "leds_id"

linda_ui_ns = cg.esphome_ns.namespace("linda_ui")
LindaUI = linda_ui_ns.class_("LindaUI", cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(LindaUI),
    cv.Required(CONF_DISPLAY_ID): cv.use_id(cg.EntityBase),
    cv.Required(CONF_API_ID): cv.use_id(cg.EntityBase),
    cv.Optional(CONF_LEDS_ID): cv.use_id(cg.EntityBase),
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
```

#### components/linda_ui/ui_theme.h

```cpp
#pragma once

#include <lvgl.h>

namespace esphome {
namespace linda_ui {

// Color scheme
#define COLOR_BG_PRIMARY      lv_color_hex(0x1a1a2e)
#define COLOR_BG_SECONDARY    lv_color_hex(0x16213e)
#define COLOR_ACCENT          lv_color_hex(0x1DB954)  // Spotify green
#define COLOR_TEXT_PRIMARY    lv_color_hex(0xFFFFFF)
#define COLOR_TEXT_SECONDARY  lv_color_hex(0xB3B3B3)
#define COLOR_HIGHLIGHT       lv_color_hex(0xFFD700)
#define COLOR_ERROR           lv_color_hex(0xFF4444)

// Dimensions
#define SCREEN_WIDTH          240
#define SCREEN_HEIGHT         240
#define SCREEN_CENTER_X       120
#define SCREEN_CENTER_Y       120

// Cover image sizes
#define COVER_SIZE_LARGE      120
#define COVER_SIZE_MEDIUM     100
#define COVER_SIZE_SMALL      60

// Animation durations (ms)
#define ANIM_DURATION_FAST    100
#define ANIM_DURATION_NORMAL  150
#define ANIM_DURATION_SLOW    300

// Font sizes (use built-in LVGL Montserrat)
#define FONT_SMALL            &lv_font_montserrat_14
#define FONT_MEDIUM           &lv_font_montserrat_18
#define FONT_LARGE            &lv_font_montserrat_24
#define FONT_XLARGE           &lv_font_montserrat_32

// Initialize theme
void init_theme();

// Create styled objects
lv_obj_t* create_rounded_panel(lv_obj_t* parent);
lv_obj_t* create_title_label(lv_obj_t* parent, const char* text);
lv_obj_t* create_subtitle_label(lv_obj_t* parent, const char* text);

}  // namespace linda_ui
}  // namespace esphome
```

#### components/linda_ui/linda_ui.h

```cpp
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
```

#### components/linda_ui/linda_ui.cpp

```cpp
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
```

#### components/linda_ui/screen_home.h

```cpp
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
```

#### components/linda_ui/screen_home.cpp

```cpp
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
```

#### components/linda_ui/screen_series.h

```cpp
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
```

#### components/linda_ui/screen_series.cpp

```cpp
#include "screen_series.h"
#include "ui_theme.h"

namespace esphome {
namespace linda_ui {

void ScreenSeries::create() {
    screen_ = lv_obj_create(nullptr);
    lv_obj_set_style_bg_color(screen_, COLOR_BG_PRIMARY, 0);

    // Header
    header_label_ = lv_label_create(screen_);
    lv_obj_set_style_text_font(header_label_, FONT_MEDIUM, 0);
    lv_obj_set_style_text_color(header_label_, COLOR_ACCENT, 0);
    lv_obj_align(header_label_, LV_ALIGN_TOP_MID, 0, 15);
    lv_label_set_text(header_label_, "Episodes");

    // Divider line
    lv_obj_t* divider = lv_obj_create(screen_);
    lv_obj_set_size(divider, 180, 2);
    lv_obj_align(divider, LV_ALIGN_TOP_MID, 0, 45);
    lv_obj_set_style_bg_color(divider, COLOR_TEXT_SECONDARY, 0);
    lv_obj_set_style_border_width(divider, 0, 0);

    // Track list (using roller for smooth scrolling)
    list_ = lv_roller_create(screen_);
    lv_obj_set_size(list_, 200, 140);
    lv_obj_align(list_, LV_ALIGN_CENTER, 0, 20);
    lv_obj_set_style_bg_color(list_, COLOR_BG_PRIMARY, 0);
    lv_obj_set_style_bg_color(list_, COLOR_BG_SECONDARY, LV_PART_SELECTED);
    lv_obj_set_style_text_color(list_, COLOR_TEXT_SECONDARY, 0);
    lv_obj_set_style_text_color(list_, COLOR_TEXT_PRIMARY, LV_PART_SELECTED);
    lv_obj_set_style_text_font(list_, FONT_MEDIUM, 0);
    lv_obj_set_style_border_width(list_, 0, 0);
    lv_roller_set_visible_row_count(list_, 4);
}

void ScreenSeries::on_rotate(int direction) {
    if (tracks_.empty()) return;

    int new_index = current_index_ + direction;
    
    if (new_index >= 0 && new_index < static_cast<int>(tracks_.size())) {
        current_index_ = new_index;
        lv_roller_set_selected(list_, current_index_, LV_ANIM_ON);
    }
}

void ScreenSeries::on_click() {
    if (on_select_ && !tracks_.empty()) {
        on_select_(current_index_);
    }
}

void ScreenSeries::set_tracks(const std::vector<linda_api::Track>& tracks) {
    tracks_ = tracks;
    current_index_ = 0;

    // Build options string for roller
    std::string options;
    for (size_t i = 0; i < tracks_.size(); i++) {
        if (i > 0) options += "\n";
        options += tracks_[i].name;
    }

    lv_roller_set_options(list_, options.c_str(), LV_ROLLER_MODE_NORMAL);
    lv_roller_set_selected(list_, 0, LV_ANIM_OFF);
}

void ScreenSeries::set_title(const std::string& title) {
    lv_label_set_text(header_label_, title.c_str());
}

}  // namespace linda_ui
}  // namespace esphome
```

#### components/linda_ui/screen_player.h

```cpp
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
```

#### components/linda_ui/screen_player.cpp

```cpp
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
```

### 3. linda_api Component

#### components/linda_api/__init__.py

```python
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_HOST, CONF_PORT

CONF_PLAYER_ID = "player_id"

linda_api_ns = cg.esphome_ns.namespace("linda_api")
LindaAPI = linda_api_ns.class_("LindaAPI", cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(LindaAPI),
    cv.Required(CONF_HOST): cv.string,
    cv.Optional(CONF_PORT, default=8095): cv.port,
    cv.Required(CONF_PLAYER_ID): cv.string,
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    
    cg.add(var.set_host(config[CONF_HOST]))
    cg.add(var.set_port(config[CONF_PORT]))
    cg.add(var.set_player_id(config[CONF_PLAYER_ID]))
    
    cg.add_library("bblanchon/ArduinoJson", "6.21.3")
```

#### components/linda_api/api_types.h

```cpp
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
```

#### components/linda_api/linda_api.h

```cpp
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
```

#### components/linda_api/linda_api.cpp

```cpp
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
```

---

## Control Concept (iPod-Style)

### Input Matrix

| Input | HOME | SERIES | PLAYING |
|-------|------|--------|---------|
| **Rotate** | Select series | Select episode | 🔊 Volume |
| **Click** | Open series | Play episode | Pause / Play |
| **Double-click** | — | ← Back (Home) | ← Back (Series) |
| **Long press (2s)** | — | — | Stop → Home |

### State Machine

```
                         ┌─────────┐
                         │  BOOT   │
                         └────┬────┘
                              │
                              ▼
                         ┌─────────┐
              ┌─────────►│  HOME   │◄─────────┐
              │          └────┬────┘          │
              │               │               │
              │ Double-       │ Click         │ Long (2s)
              │ click         ▼               │
              │          ┌─────────┐          │
              │          │ SERIES  │          │
              │          └────┬────┘          │
              │               │               │
              │ Double-       │ Click         │
              │ click         ▼               │
              │          ┌─────────┐          │
              └──────────│ PLAYING │──────────┘
                         └─────────┘
```

### RGB LED Feedback

| State | LED Behavior |
|-------|--------------|
| Idle / Boot | Soft blue breathing |
| Navigating | Show position (1 of 5 LEDs active) |
| Playing | Green pulsing |
| Volume change | Fill indicator |
| Error / Offline | Red blinking |

---

## Implementation Phases

### Phase 1: Hardware Foundation
1. [ ] Create ESPHome project structure
2. [ ] Implement linda_display component
3. [ ] Test display with "Hello World"
4. [ ] Test rotary encoder input
5. [ ] Test RGB LEDs

### Phase 2: UI Screens
6. [ ] Implement UI theme (colors, fonts)
7. [ ] Create Home screen (static)
8. [ ] Create Series screen (static)
9. [ ] Create Player screen (static)
10. [ ] Implement screen navigation

### Phase 3: Music Assistant Integration
11. [ ] Implement linda_api component
12. [ ] Load and display playlists
13. [ ] Load and display tracks
14. [ ] Implement playback control
15. [ ] Implement volume control

### Phase 4: Polish
16. [ ] Add animations
17. [ ] Implement LED feedback
18. [ ] Load cover images (optional)
19. [ ] Error handling
20. [ ] OTA update testing

---

## Configuration Checklist

Fill in before starting:

```yaml
# secrets.yaml
wifi_ssid: "_______________"
wifi_password: "_______________"
ap_password: "_______________"
api_key: "_______________"
ota_password: "_______________"

ma_host: "_______________"      # e.g., "192.168.1.100" or "homeassistant.local"
ma_player_id: "_______________" # Linda's speaker ID in Music Assistant
```

---

## References

- **CrowPanel Wiki:** https://www.elecrow.com/wiki/CrowPanel_1.28inch-HMI_ESP32_Rotary_Display.html
- **CrowPanel GitHub:** https://github.com/Elecrow-RD/CrowPanel-1.28inch-HMI-ESP32-Rotary-Display-240-240-IPS-Round-Touch-Knob-Screen
- **ESPHome Custom Components:** https://esphome.io/custom/custom_component.html
- **LVGL Docs:** https://docs.lvgl.io/8.3/
- **SquareLine Studio:** https://squareline.io
- **Music Assistant API:** https://music-assistant.io/integration/api/
- **LovyanGFX:** https://github.com/lovyan03/LovyanGFX
- **homeThing (Inspiration):** https://github.com/landonr/homeThing