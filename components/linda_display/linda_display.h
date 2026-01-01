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
