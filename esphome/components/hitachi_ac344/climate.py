import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import climate_ir
from esphome.const import CONF_ID

AUTO_LOAD = ["climate_ir"]

climate_ns = cg.esphome_ns.namespace("climate")
hitachi_ac344_ns = cg.esphome_ns.namespace("hitachi_ac344")
HitachiClimate = hitachi_ac344_ns.class_("HitachiClimate", climate_ir.ClimateIR)

CONF_HITACHI_HORIZONTAL_DEFAULT = "horizontal_default"
HORIZONTAL_DIRECTIONS = {
    "left_max": hitachi_ac344_ns.HITACHI_AC344_SWINGH_LEFT_MAX,
    "left": hitachi_ac344_ns.HITACHI_AC344_SWINGH_LEFT,
    "middle": hitachi_ac344_ns.HITACHI_AC344_SWINGH_MIDDLE,
    "right": hitachi_ac344_ns.HITACHI_AC344_SWINGH_RIGHT,
    "right_max": hitachi_ac344_ns.HITACHI_AC344_SWINGH_RIGHT_MAX,
}

HITACHI_SWING_MODES = {
    "horizontal": climate_ns.CLIMATE_SWING_HORIZONTAL,
    "vertical": climate_ns.CLIMATE_SWING_VERTICAL,
    "both": climate_ns.CLIMATE_SWING_BOTH,
    "off": climate_ns.CLIMATE_SWING_OFF,
}

HITACHI_FAN_MODES = {
    "auto": climate_ns.CLIMATE_FAN_AUTO,
    "low": climate_ns.CLIMATE_FAN_LOW,
    "medium": climate_ns.CLIMATE_FAN_MEDIUM,
    "high": climate_ns.CLIMATE_FAN_HIGH,
    "quiet": climate_ns.CLIMATE_FAN_QUIET,
}

CONF_CUSTOM_COOL = "custom_cool"
CONF_CUSTOM_COOL_TEMPERATURE = "temperature"
CONF_CUSTOM_COOL_SWING = "swing"
CONF_CUSTOM_COOL_FAN_MODE = "fan_mode"
CUSTOM_COOL_SCHEMA = cv.Schema(
    {
        cv.Optional(CONF_CUSTOM_COOL_TEMPERATURE, default=28): cv.int_,
        cv.Optional(CONF_CUSTOM_COOL_SWING, default="horizontal"): cv.enum(HITACHI_SWING_MODES),
        cv.Optional(CONF_CUSTOM_COOL_FAN_MODE, default="auto"): cv.enum(HITACHI_FAN_MODES),
    }
)

CONF_CUSTOM_HEAT = "custom_heat"
CONF_CUSTOM_HEAT_TEMPERATURE = "temperature"
CONF_CUSTOM_HEAT_SWING = "swing"
CONF_CUSTOM_HEAT_FAN_MODE = "fan_mode"
CUSTOM_HEAT_SCHEMA = cv.Schema(
    {
        cv.Optional(CONF_CUSTOM_HEAT_TEMPERATURE, default=24): cv.int_,
        cv.Optional(CONF_CUSTOM_HEAT_SWING, default="off"): cv.enum(HITACHI_SWING_MODES),
        cv.Optional(CONF_CUSTOM_HEAT_FAN_MODE, default="auto"): cv.enum(HITACHI_FAN_MODES),
    }
)

CONF_CUSTOM_DRY = "custom_dry"
CONF_CUSTOM_DRY_TEMPERATURE = "temperature"
CONF_CUSTOM_DRY_SWING = "swing"
CONF_CUSTOM_DRY_FAN_MODE = "fan_mode"
CUSTOM_DRY_SCHEMA = cv.Schema(
    {
        cv.Optional(CONF_CUSTOM_DRY_TEMPERATURE, default=28): cv.int_,
        cv.Optional(CONF_CUSTOM_DRY_SWING, default="horizontal"): cv.enum(HITACHI_SWING_MODES),
        cv.Optional(CONF_CUSTOM_DRY_FAN_MODE, default="low"): cv.enum(HITACHI_FAN_MODES),
    }
)

CONF_CUSTOM_FAN_ONLY = "custom_fan_only"
CONF_CUSTOM_FAN_ONLY_SWING = "swing"
CONF_CUSTOM_FAN_ONLY_FAN_MODE = "fan_mode"
CUSTOM_FAN_ONLY_SCHEMA = cv.Schema(
    {
        cv.Optional(CONF_CUSTOM_FAN_ONLY_SWING, default="off"): cv.enum(HITACHI_SWING_MODES),
        cv.Optional(CONF_CUSTOM_FAN_ONLY_FAN_MODE, default="low"): cv.enum(HITACHI_FAN_MODES),
    }
)

CONF_MILDEWPROOF = "mildewproof"

CONFIG_SCHEMA = climate_ir.CLIMATE_IR_WITH_RECEIVER_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(HitachiClimate),
        cv.Optional(CONF_HITACHI_HORIZONTAL_DEFAULT, default="middle"): cv.enum(
            HORIZONTAL_DIRECTIONS
        ),
        cv.Optional(CONF_MILDEWPROOF, default=False): cv.boolean,
        cv.Optional(CONF_CUSTOM_COOL): CUSTOM_COOL_SCHEMA,
        cv.Optional(CONF_CUSTOM_HEAT): CUSTOM_HEAT_SCHEMA,
        cv.Optional(CONF_CUSTOM_DRY): CUSTOM_DRY_SCHEMA,
        cv.Optional(CONF_CUSTOM_FAN_ONLY): CUSTOM_FAN_ONLY_SCHEMA,
    }
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await climate_ir.register_climate_ir(var, config)
    cg.add(var.set_horizontal_default(config[CONF_HITACHI_HORIZONTAL_DEFAULT]))
    cg.add(var.set_mildewproof(config[CONF_MILDEWPROOF]))

    if CONF_CUSTOM_COOL in config:
        cg.add(var.set_custom_cool(config[CONF_CUSTOM_COOL][CONF_CUSTOM_COOL_TEMPERATURE], config[CONF_CUSTOM_COOL][CONF_CUSTOM_COOL_SWING], config[CONF_CUSTOM_COOL][CONF_CUSTOM_COOL_FAN_MODE]))
    else:
        cg.add(var.set_custom_cool(28, climate_ns.CLIMATE_SWING_HORIZONTAL, climate_ns.CLIMATE_FAN_AUTO))

    if CONF_CUSTOM_HEAT in config:
        cg.add(var.set_custom_heat(config[CONF_CUSTOM_HEAT][CONF_CUSTOM_HEAT_TEMPERATURE], config[CONF_CUSTOM_HEAT][CONF_CUSTOM_HEAT_SWING], config[CONF_CUSTOM_HEAT][CONF_CUSTOM_HEAT_FAN_MODE]))
    else:
        cg.add(var.set_custom_heat(24, climate_ns.CLIMATE_SWING_OFF, climate_ns.CLIMATE_FAN_AUTO))

    if CONF_CUSTOM_DRY in config:
        cg.add(var.set_custom_dry(config[CONF_CUSTOM_DRY][CONF_CUSTOM_DRY_TEMPERATURE], config[CONF_CUSTOM_DRY][CONF_CUSTOM_DRY_SWING], config[CONF_CUSTOM_DRY][CONF_CUSTOM_DRY_FAN_MODE]))
    else:
        cg.add(var.set_custom_dry(28, climate_ns.CLIMATE_SWING_HORIZONTAL, climate_ns.CLIMATE_FAN_LOW))

    if CONF_CUSTOM_FAN_ONLY in config:
        cg.add(var.set_custom_fan_only(config[CONF_CUSTOM_FAN_ONLY][CONF_CUSTOM_FAN_ONLY_SWING], config[CONF_CUSTOM_FAN_ONLY][CONF_CUSTOM_FAN_ONLY_FAN_MODE]))
    else:
        cg.add(var.set_custom_fan_only(climate_ns.CLIMATE_SWING_OFF, climate_ns.CLIMATE_FAN_LOW))
