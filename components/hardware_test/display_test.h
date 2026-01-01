#pragma once

#include "esphome/core/component.h"
#include "esphome/core/log.h"

#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <lvgl.h>

namespace esphome {
namespace hardware_test {

// Custom LGFX class for GC9A01 on CrowPanel
class LGFX_CrowPanel : public lgfx::LGFX_Device {
public:
    lgfx::Panel_GC9A01 _panel_instance;
    lgfx::Bus_SPI _bus_instance;

    LGFX_CrowPanel();
};

class DisplayTest : public Component {
public:
    static const uint16_t SCREEN_WIDTH = 240;
    static const uint16_t SCREEN_HEIGHT = 240;

    void setup() override;
    void loop() override;
    float get_setup_priority() const override { return setup_priority::PROCESSOR; }

protected:
    LGFX_CrowPanel gfx_;

    // LVGL display buffer
    static lv_disp_draw_buf_t draw_buf_;
    static lv_color_t buf1_[SCREEN_WIDTH * 20];
    static lv_color_t buf2_[SCREEN_WIDTH * 20];
    static lv_disp_drv_t disp_drv_;
    static LGFX_CrowPanel* gfx_instance_;

    static void lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_p);
    void create_hello_world_ui();
};

}  // namespace hardware_test
}  // namespace esphome
