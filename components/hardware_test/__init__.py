import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID
from esphome.core import CORE
import os

CODEOWNERS = ["@tobias"]

hardware_test_ns = cg.esphome_ns.namespace("hardware_test")
DisplayTest = hardware_test_ns.class_("DisplayTest", cg.Component)

CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(DisplayTest),
}).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    cg.add_library("lvgl/lvgl", "8.3.11")
    # Use latest LovyanGFX from GitHub for ESP-IDF 5.x compatibility
    cg.add_library(
        "LovyanGFX",
        None,
        "https://github.com/lovyan03/LovyanGFX.git#master"
    )

    # Get the path to this component's directory for lv_conf.h
    component_dir = os.path.dirname(__file__)
    cg.add_build_flag(f"-I{component_dir}")
    cg.add_build_flag("-DLV_CONF_INCLUDE_SIMPLE")
