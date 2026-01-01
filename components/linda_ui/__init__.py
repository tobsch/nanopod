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
