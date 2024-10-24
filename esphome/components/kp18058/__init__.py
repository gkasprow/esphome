import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.const import (
    CONF_CLOCK_PIN,
    CONF_DATA_PIN,
    CONF_ID,
)

MULTI_CONF = True

CONF_CW_CURRENT  = "cw_current"
CONF_RGB_CURRENT = "rgb_current"

KP18058_ns = cg.esphome_ns.namespace("kp18058")
KP18058    = KP18058_ns.class_("kp18058", cg.Component)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(KP18058),
        cv.Required(CONF_DATA_PIN):       pins.gpio_output_pin_schema,
        cv.Required(CONF_CLOCK_PIN):      pins.gpio_output_pin_schema,
        cv.Optional(CONF_CW_CURRENT,  default=5): cv.int_range(min=0, max=31),
        cv.Optional(CONF_RGB_CURRENT, default=5): cv.int_range(min=0, max=31),
    }
).extend(cv.COMPONENT_SCHEMA)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    cg.add(var.set_cw_current(config[CONF_CW_CURRENT]))
    cg.add(var.set_rgb_current(config[CONF_RGB_CURRENT]))

    data = await cg.gpio_pin_expression(config[CONF_DATA_PIN])
    cg.add(var.set_data_pin(data))
    clock = await cg.gpio_pin_expression(config[CONF_CLOCK_PIN])
    cg.add(var.set_clock_pin(clock))


