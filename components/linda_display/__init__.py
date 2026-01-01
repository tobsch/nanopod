import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import spi
from esphome.const import CONF_ID, CONF_CS_PIN, CONF_DC_PIN, CONF_RESET_PIN
import os

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
    # Use latest LovyanGFX from GitHub for ESP-IDF 5.x compatibility
    cg.add_library(
        "LovyanGFX",
        None,
        "https://github.com/lovyan03/LovyanGFX.git#master"
    )

    # Get the path to the project root for lv_conf.h
    component_dir = os.path.dirname(__file__)
    project_root = os.path.dirname(os.path.dirname(component_dir))
    cg.add_build_flag(f"-I{project_root}")
    cg.add_build_flag("-DLV_CONF_INCLUDE_SIMPLE")
