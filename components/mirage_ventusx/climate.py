import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import climate
from esphome.const import CONF_ID

AUTO_LOAD = ["remote_transmitter", "remote_receiver"]
DEPENDENCIES = ["climate"]
CODEOWNERS = ["@bitflipper11"]

ventusx_ns = cg.esphome_ns.namespace("mirage_ventusx")
VentusXClimate = ventusx_ns.class_("MirageVentusXClimate", climate.Climate, cg.Component)

CONFIG_SCHEMA = climate.CLIMATE_SCHEMA.extend({
    cv.GenerateID(): cv.declare_id(VentusXClimate),
}).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await climate.register_climate(var, config)
