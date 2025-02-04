import esphome.codegen as cg
from esphome.components import switch
import esphome.config_validation as cv
from esphome.const import (
    DEVICE_CLASS_SWITCH,
)
from .. import (
    DeepSleepComponent,
    deep_sleep_ns,
)

CONF_DEEP_SLEEP_ID = "deep_sleep_id"
CONF_GUARD = "guard"
ICON_SLEEP = "mdi:sleep"

GuardSwitch = deep_sleep_ns.class_("GuardSwitch", switch.Switch)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_DEEP_SLEEP_ID): cv.use_id(DeepSleepComponent),
        cv.Optional(CONF_GUARD): switch.switch_schema(
            GuardSwitch,
            device_class=DEVICE_CLASS_SWITCH,
            icon=ICON_SLEEP,
        ),
    }
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_DEEP_SLEEP_ID])

    if CONF_GUARD in config:
        s = await switch.new_switch(config[CONF_GUARD])
        await cg.register_parented(s, parent)
        cg.add(parent.set_guard_switch(s))
