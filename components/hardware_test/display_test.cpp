#include "display_test.h"

namespace esphome {
namespace hardware_test {

static const char *TAG = "display_test";

// LGFX constructor
LGFX_CrowPanel::LGFX_CrowPanel() {
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

// Static member definitions
lv_disp_draw_buf_t DisplayTest::draw_buf_;
lv_color_t DisplayTest::buf1_[DisplayTest::SCREEN_WIDTH * 20];
lv_color_t DisplayTest::buf2_[DisplayTest::SCREEN_WIDTH * 20];
lv_disp_drv_t DisplayTest::disp_drv_;
LGFX_CrowPanel* DisplayTest::gfx_instance_ = nullptr;

void DisplayTest::setup() {
    ESP_LOGI(TAG, "Initializing display test...");

    // Initialize display
    gfx_.init();
    gfx_.setRotation(0);
    gfx_.setBrightness(255);
    gfx_.fillScreen(TFT_BLACK);

    gfx_instance_ = &gfx_;

    ESP_LOGI(TAG, "Display initialized, starting LVGL...");

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

    ESP_LOGI(TAG, "LVGL initialized, creating UI...");

    // Create Hello World UI
    create_hello_world_ui();

    ESP_LOGI(TAG, "Display test setup complete!");
}

void DisplayTest::loop() {
    lv_timer_handler();
}

void DisplayTest::lvgl_flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_p) {
    if (gfx_instance_ == nullptr) return;

    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    gfx_instance_->startWrite();
    gfx_instance_->setAddrWindow(area->x1, area->y1, w, h);
    gfx_instance_->writePixels((lgfx::rgb565_t *)&color_p->full, w * h);
    gfx_instance_->endWrite();

    lv_disp_flush_ready(drv);
}

void DisplayTest::create_hello_world_ui() {
    // Get the active screen
    lv_obj_t* scr = lv_scr_act();

    // Set dark background
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x1a1a2e), 0);

    // Create circular border (to match the round display)
    lv_obj_t* circle = lv_obj_create(scr);
    lv_obj_set_size(circle, 220, 220);
    lv_obj_center(circle);
    lv_obj_set_style_radius(circle, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(circle, lv_color_hex(0x16213e), 0);
    lv_obj_set_style_border_width(circle, 3, 0);
    lv_obj_set_style_border_color(circle, lv_color_hex(0x1DB954), 0);

    // Create "Hello" label
    lv_obj_t* hello_label = lv_label_create(circle);
    lv_obj_set_style_text_font(hello_label, &lv_font_montserrat_24, 0);
    lv_obj_set_style_text_color(hello_label, lv_color_hex(0xB3B3B3), 0);
    lv_label_set_text(hello_label, "Hello");
    lv_obj_align(hello_label, LV_ALIGN_CENTER, 0, -30);

    // Create "NanoPod" label (larger, accent color)
    lv_obj_t* name_label = lv_label_create(circle);
    lv_obj_set_style_text_font(name_label, &lv_font_montserrat_32, 0);
    lv_obj_set_style_text_color(name_label, lv_color_hex(0x1DB954), 0);
    lv_label_set_text(name_label, "NanoPod");
    lv_obj_align(name_label, LV_ALIGN_CENTER, 0, 10);

    // Create instruction label at bottom
    lv_obj_t* instruction_label = lv_label_create(circle);
    lv_obj_set_style_text_font(instruction_label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(instruction_label, lv_color_hex(0x666666), 0);
    lv_label_set_text(instruction_label, "Turn & Click to test");
    lv_obj_align(instruction_label, LV_ALIGN_CENTER, 0, 60);

    ESP_LOGI(TAG, "Hello World UI created");
}

}  // namespace hardware_test
}  // namespace esphome
