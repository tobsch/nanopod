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
