from esphome import pins
import esphome.codegen as cg
from esphome.components import i2c, touchscreen
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_INTERRUPT_PIN, CONF_RESET_PIN, CONF_NOISE_LEVEL

from .. import gt911_ns

GT911ButtonListener = gt911_ns.class_("GT911ButtonListener")
GT911Touchscreen = gt911_ns.class_(
    "GT911Touchscreen",
    touchscreen.Touchscreen,
    i2c.I2CDevice,
)

CONFIG_SCHEMA = touchscreen.TOUCHSCREEN_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(GT911Touchscreen),
        cv.Optional(CONF_INTERRUPT_PIN): pins.internal_gpio_input_pin_schema,
        cv.Optional(CONF_NOISE_LEVEL, default=-1): cv.int_range(min=-1, max=15),
        cv.Optional(CONF_RESET_PIN): pins.gpio_output_pin_schema,
    }
).extend(i2c.i2c_device_schema(0x5D))


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await touchscreen.register_touchscreen(var, config)
    await i2c.register_i2c_device(var, config)

    if interrupt_pin := config.get(CONF_INTERRUPT_PIN):
        cg.add(var.set_interrupt_pin(await cg.gpio_pin_expression(interrupt_pin)))
    if reset_pin := config.get(CONF_RESET_PIN):
        cg.add(var.set_reset_pin(await cg.gpio_pin_expression(reset_pin)))

    cg.add(var.set_noise_level(config[CONF_NOISE_LEVEL]))

