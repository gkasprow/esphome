from esphome import automation, pins
import esphome.codegen as cg
from esphome.components import esp32_rmt, remote_base
import esphome.config_validation as cv
from esphome.const import CONF_CARRIER_DUTY_PERCENT, CONF_ID, CONF_PIN, CONF_RMT_CHANNEL
from esphome.core import CORE

AUTO_LOAD = ["remote_base"]

CONF_CLOCK_DIVIDER = "clock_divider"
CONF_CLOCK_RESOLUTION = "clock_resolution"
CONF_ON_TRANSMIT = "on_transmit"
CONF_ON_COMPLETE = "on_complete"
CONF_WITH_DMA = "with_dma"
CONF_ONE_WIRE = "one_wire"

remote_transmitter_ns = cg.esphome_ns.namespace("remote_transmitter")
RemoteTransmitterComponent = remote_transmitter_ns.class_(
    "RemoteTransmitterComponent", remote_base.RemoteTransmitterBase, cg.Component
)


def validate_config(config):
    if CORE.is_esp32:
        if esp32_rmt.use_new_rmt_driver():
            if CONF_CLOCK_DIVIDER in config:
                raise cv.Invalid(
                    "clock_divider not available with the new RMT driver, use clock_resolution instead"
                )
        else:
            if CONF_CLOCK_RESOLUTION in config:
                raise cv.Invalid(
                    "clock_resolution not available with the legacy RMT driver, use clock_divider instead"
                )
            if CONF_ONE_WIRE in config:
                raise cv.Invalid("one_wire not available with the legacy RMT driver")
            if CONF_WITH_DMA in config:
                raise cv.Invalid("with_dma not available with the legacy RMT driver")


MULTI_CONF = True
FINAL_VALIDATE_SCHEMA = validate_config
CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(RemoteTransmitterComponent),
        cv.Required(CONF_PIN): pins.gpio_output_pin_schema,
        cv.Required(CONF_CARRIER_DUTY_PERCENT): cv.All(
            cv.percentage_int, cv.Range(min=1, max=100)
        ),
        cv.Optional(CONF_CLOCK_RESOLUTION): cv.All(
            cv.only_on_esp32, esp32_rmt.validate_clock_resolution()
        ),
        cv.Optional(CONF_CLOCK_DIVIDER): cv.All(
            cv.only_on_esp32, cv.Range(min=1, max=255)
        ),
        cv.Optional(CONF_ONE_WIRE): cv.boolean,
        cv.Optional(CONF_WITH_DMA): cv.boolean,
        cv.Optional(CONF_RMT_CHANNEL): esp32_rmt.validate_rmt_channel(tx=True),
        cv.Optional(CONF_ON_TRANSMIT): automation.validate_automation(single=True),
        cv.Optional(CONF_ON_COMPLETE): automation.validate_automation(single=True),
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    pin = await cg.gpio_pin_expression(config[CONF_PIN])
    if CORE.is_esp32:
        new_driver = esp32_rmt.use_new_rmt_driver()
        rmt_channel = config.get(CONF_RMT_CHANNEL, None)
        if not new_driver and rmt_channel is not None:
            var = cg.new_Pvariable(config[CONF_ID], pin, rmt_channel)
        else:
            var = cg.new_Pvariable(config[CONF_ID], pin)
        if CONF_CLOCK_DIVIDER in config:
            cg.add(var.set_clock_divider(config[CONF_CLOCK_DIVIDER]))
        if CONF_CLOCK_RESOLUTION in config:
            cg.add(var.set_clock_resolution(config[CONF_CLOCK_RESOLUTION]))
        if new_driver:
            if CONF_WITH_DMA in config:
                cg.add(var.set_with_dma(config[CONF_WITH_DMA]))
            if CONF_ONE_WIRE in config:
                cg.add(var.set_one_wire(config[CONF_ONE_WIRE]))
    else:
        var = cg.new_Pvariable(config[CONF_ID], pin)
    await cg.register_component(var, config)

    cg.add(var.set_carrier_duty_percent(config[CONF_CARRIER_DUTY_PERCENT]))

    if on_transmit_config := config.get(CONF_ON_TRANSMIT):
        await automation.build_automation(
            var.get_transmit_trigger(), [], on_transmit_config
        )

    if on_complete_config := config.get(CONF_ON_COMPLETE):
        await automation.build_automation(
            var.get_complete_trigger(), [], on_complete_config
        )
