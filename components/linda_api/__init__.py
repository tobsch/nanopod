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
