# Linda's Audiobook Controller - Development Notes

## Project Overview
An iPod-style audiobook controller for children using the CrowPanel 1.28" HMI ESP32 Rotary Display with ESPHome and LVGL.

## Hardware: CrowPanel 1.28" ESP32-S3
- **Display**: GC9A01A 240x240 round LCD
- **MCU**: ESP32-S3 with 16MB Flash, 8MB PSRAM
- **Input**: Rotary encoder with button (GPIO45/42 for rotation, GPIO41 for button)
- **LEDs**: 5x WS2812 RGB LEDs on GPIO48
- **Touch**: CST816S on I2C (GPIO6/7, INT=GPIO5, RST=GPIO13)

### Critical: Power Rail Configuration
The CrowPanel requires explicit power rail activation before the display works:
```yaml
output:
  - platform: gpio
    id: lcd_power
    pin: GPIO1      # LCD power rail

  - platform: gpio
    id: backlight_power
    pin: GPIO2      # Backlight power rail

  - platform: gpio
    id: backlight_pwm
    pin: GPIO46     # Backlight PWM
```

Enable in `on_boot`:
```yaml
on_boot:
  priority: 600
  then:
    - switch.turn_on: lcd_power_switch
    - switch.turn_on: backlight_power_switch
    - delay: 100ms
    - switch.turn_on: backlight
```

## Key Learnings

### 1. LVGL Font Naming Collision
**Problem**: Umlauts and special characters not displaying.

**Root Cause**: Using font IDs like `montserrat_24` causes ESPHome's LVGL component to use LVGL's built-in `lv_font_montserrat_24` instead of the custom font. Built-in LVGL fonts only include ASCII (0x20-0x7F).

**Solution**: Use unique font IDs that don't match LVGL built-in names:
```yaml
# BAD - conflicts with LVGL built-in fonts
font:
  - file: "gfonts://Montserrat"
    id: montserrat_24  # Will use lv_font_montserrat_24 instead!

# GOOD - unique names, uses custom font with extended glyphs
font:
  - file: "gfonts://Montserrat"
    id: linda_font_24
    size: 24
    bpp: 4
    glyphsets:
      - GF_Latin_Kernel
      - GF_Latin_Core  # Includes umlauts: äöüÄÖÜß
```

### 2. ESPHome Display Platform
**Problem**: LovyanGFX has compatibility issues with ESP-IDF 5.x.

**Solution**: Use ESPHome's native `ili9xxx` platform with `GC9A01A` model:
```yaml
display:
  - platform: ili9xxx
    model: GC9A01A
    cs_pin: GPIO9
    dc_pin: GPIO3
    reset_pin: GPIO14
    invert_colors: true
    data_rate: 40MHz
```

### 3. LVGL Roller Updates
**Problem**: `lvgl.roller.update` doesn't support changing options dynamically.

**Solution**: Use direct LVGL C API calls in lambdas:
```yaml
- lambda: |-
    lv_roller_set_options(id(episode_roller)->obj, "Option1\nOption2\nOption3", LV_ROLLER_MODE_NORMAL);
    lv_roller_set_selected(id(episode_roller)->obj, 0, LV_ANIM_ON);
```

### 4. LVGL Label Updates
Use the `lvgl.label.update` action for dynamic text:
```yaml
- lvgl.label.update:
    id: my_label
    text: !lambda 'return "Dynamic text";'
```

### 5. LVGL Arc/Bar Indicator Styling
Nested structure required for indicator colors:
```yaml
# BAD
indicator_color: 0x1DB954

# GOOD
indicator:
  arc_color: 0x1DB954  # for arc widgets
  bg_color: 0x1DB954   # for bar widgets
```

### 6. LVGL Roller Options Format
Use YAML list format, not newline-separated strings:
```yaml
# BAD
options: "Option1\nOption2\nOption3"

# GOOD
options:
  - "Option 1"
  - "Option 2"
  - "Option 3"
```

### 7. HTTP Request Headers
In ESPHome's `http_request` component, use `json:` for POST body:
```yaml
- http_request.post:
    url: "http://host/api/endpoint"
    json:
      key: !lambda 'return some_value;'
```

### 8. CST816S Touchscreen with ESPHome
The CST816 platform supports gesture detection via coordinate tracking:
```yaml
touchscreen:
  - platform: cst816
    id: touch
    i2c_id: bus_touch
    interrupt_pin: GPIO5
    reset_pin: GPIO13
    on_touch:
      - lambda: |-
          id(touch_start_x) = touch.x;
          id(touch_start_y) = touch.y;
    on_release:
      - script.execute: handle_touch_gesture
```
To calculate swipe direction, compare start coordinates with end position from `id(touch)->get_touch_points()[0]`.

## File Structure
```
├── linda-controller.yaml   # Main application
├── hardware-test.yaml      # Hardware verification config
├── secrets.yaml            # WiFi, API keys, MA config
└── claude.md               # This file
```

## Music Assistant Integration
API endpoints (adjust for your MA version):
- Play: `POST /api/players/{player_id}/play`
- Pause: `POST /api/players/{player_id}/pause`
- Stop: `POST /api/players/{player_id}/stop`
- Volume: `POST /api/players/{player_id}/volume_set` with JSON body

## Navigation State Machine

Both rotary encoder and touch gestures are supported:

```
HOME (artist carousel)
  │
  ├── Turn / Swipe L/R: cycle through artists
  ├── Click / Tap / Swipe Down: → ALBUMS
  └── Swipe Up: (no action)

ALBUMS (album list)
  │
  ├── Turn / Swipe L/R: scroll albums
  ├── Click / Tap / Swipe Down: → PLAYER (start playback)
  ├── Long press / Swipe Up: → HOME
  └── Double-click: → HOME

PLAYER (now playing)
  │
  ├── Turn / Swipe L/R: adjust volume
  ├── Click / Tap: toggle play/pause
  ├── Long press / Swipe Up: stop → ALBUMS
  └── Swipe Down: toggle play/pause
```

### Touch Gesture Detection
Gestures are detected by tracking touch start/end coordinates:
- **Swipe threshold**: 30px minimum movement
- **Tap detection**: <20px movement, <300ms duration
- Horizontal vs vertical determined by larger delta
