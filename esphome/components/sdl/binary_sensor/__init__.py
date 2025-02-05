import esphome.codegen as cg
from esphome.components import binary_sensor
import esphome.config_validation as cv
from esphome.const import CONF_KEY

from .. import SDL_KEYMAP
from ..display import CONF_SDL_ID, Sdl, sdl_ns

CODEOWNERS = ["@bdm310"]

SdlBinarySensor = sdl_ns.class_(
    "SdlBinarySensor", binary_sensor.BinarySensor, cg.Component
)

CONFIG_SCHEMA = (
    binary_sensor.binary_sensor_schema(SdlBinarySensor)
    .extend(
        {
            cv.Required(CONF_KEY): cv.enum(SDL_KEYMAP),
            cv.GenerateID(CONF_SDL_ID): cv.use_id(Sdl),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
)


async def to_code(config):
    var = await binary_sensor.new_binary_sensor(config)
    await cg.register_component(var, config)
    await cg.register_parented(var, config[CONF_SDL_ID])

    cg.add(var.set_key(config[CONF_KEY]))
