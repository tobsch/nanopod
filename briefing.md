# NanoPod

## Project Overview

An iPod-inspired audiobook controller for children. Simple audiobook playback on a room speaker - no phone needed, no complicated apps.

### Features
- Turn + Click navigation like a classic iPod
- Touch gestures (swipe/tap) as alternative input
- Cover art display from Music Assistant
- Progress indicator and volume control
- OTA firmware updates

### Key Insight: Pure YAML, No C++ Needed

**Original plan:** Custom C++ components for display, UI, and API integration.

**Actual implementation:** 100% ESPHome YAML using native components:
- `ili9xxx` platform for GC9A01A display
- `lvgl` component for UI widgets
- `cst816` platform for touch input
- `http_request` for Music Assistant API
- `online_image` for cover art

ESPHome's native LVGL integration is powerful enough for complex UIs without any custom C++ code.

---

## Hardware

### CrowPanel 1.28" HMI ESP32 Rotary Display

| Component | Specification |
|-----------|---------------|
| MCU | ESP32-S3R8, Dual-Core 240MHz |
| Flash | 16MB |
| PSRAM | 8MB |
| Display | GC9A01A 240x240 round IPS |
| Touch | CST816S capacitive |
| Input | Rotary encoder + button |
| LEDs | 5x WS2812B RGB |

**Links:**
- [Product Page](https://www.elecrow.com/crowpanel-1-28-inch-hmi-esp32-rotary-display.html)
- [Wiki](https://www.elecrow.com/wiki/CrowPanel_1.28inch-HMI_ESP32_Rotary_Display.html)

### Pin Mapping

| Function | GPIO |
|----------|------|
| Display SCLK | 10 |
| Display MOSI | 11 |
| Display CS | 9 |
| Display DC | 3 |
| Display RST | 14 |
| Backlight PWM | 46 |
| LCD Power | 1 |
| Backlight Power | 2 |
| Touch SDA | 6 |
| Touch SCL | 7 |
| Touch INT | 5 |
| Touch RST | 13 |
| Encoder A | 45 |
| Encoder B | 42 |
| Encoder Button | 41 |
| RGB LEDs | 48 |

---

## Software Architecture

### Pure ESPHome YAML

```
┌─────────────────────────────────────────────────────────────┐
│                     nanopod.yaml                            │
│                                                             │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────┐ │
│  │  ili9xxx    │  │    lvgl     │  │   http_request      │ │
│  │  (display)  │  │    (UI)     │  │   (Music Assistant) │ │
│  └─────────────┘  └─────────────┘  └─────────────────────┘ │
│                                                             │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────┐ │
│  │   cst816    │  │   rotary    │  │   online_image      │ │
│  │   (touch)   │  │  (encoder)  │  │   (cover art)       │ │
│  └─────────────┘  └─────────────┘  └─────────────────────┘ │
└─────────────────────────────────────────────────────────────┘
```

### File Structure

```
nanopod/
├── nanopod.yaml          # Main ESPHome configuration (everything!)
├── hardware-test.yaml    # Hardware verification config
├── secrets.yaml          # WiFi, API keys, MA config
├── squareline/           # SquareLine Studio project (reference)
└── images/               # Documentation images
```

---

## Navigation

### Input Methods

Both rotary encoder and touch gestures work simultaneously:

| Action | Rotary | Touch |
|--------|--------|-------|
| Previous | Turn left | Swipe left |
| Next | Turn right | Swipe right |
| Select/Play | Click | Tap / Swipe down |
| Back | Double-click / Long press | Swipe up |

### State Machine

```
HOME (artist carousel)
  │
  ├── Turn / Swipe L/R → cycle artists
  └── Click / Tap → ALBUMS

ALBUMS (album list)
  │
  ├── Turn / Swipe L/R → scroll albums
  ├── Click / Tap → PLAYER (start playback)
  └── Swipe up / Long press → HOME

PLAYER (now playing)
  │
  ├── Turn / Swipe L/R → volume
  ├── Click / Tap → play/pause
  └── Swipe up / Long press → stop, back to ALBUMS
```

---

## Configuration

### secrets.yaml

```yaml
wifi_ssid: "YourWiFi"
wifi_password: "YourPassword"
ap_password: "fallback123"
api_key: "your-esphome-api-key"
ota_password: "your-ota-password"

ma_url: "http://music-assistant:8095"
ma_player: "your_player_id"
```

---

## Key Learnings

### 1. No C++ Needed
ESPHome's native LVGL component handles everything. Scripts with lambdas provide all necessary logic.

### 2. Power Rail Activation
CrowPanel requires explicit GPIO activation for display power:
```yaml
on_boot:
  - output.turn_on: lcd_power
  - output.turn_on: backlight_power
```

### 3. Font ID Collision
Don't use `montserrat_24` - conflicts with LVGL built-in fonts. Use unique IDs like `nanopod_font_24`.

### 4. ESP-IDF Framework
Use `esp-idf` framework (not Arduino) for better LVGL/PSRAM support:
```yaml
esp32:
  framework:
    type: esp-idf
```

### 5. Touch Gesture Detection
Track coordinates in `on_touch` and calculate delta in `on_release`:
```yaml
on_touch:
  - lambda: 'id(touch_start_x) = touch.x;'
on_release:
  - script.execute: handle_gesture
```

---

## References

- [ESPHome LVGL](https://esphome.io/components/lvgl/)
- [ESPHome ili9xxx](https://esphome.io/components/display/ili9xxx.html)
- [Music Assistant](https://music-assistant.io/)
- [CrowPanel Wiki](https://www.elecrow.com/wiki/CrowPanel_1.28inch-HMI_ESP32_Rotary_Display.html)
