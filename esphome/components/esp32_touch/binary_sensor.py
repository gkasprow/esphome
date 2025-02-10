import esphome.codegen as cg
from esphome.components import binary_sensor
import esphome.config_validation as cv
from esphome.const import (
    CONF_ID,
    CONF_INITIAL_VALUE,
    CONF_MODE,
    CONF_PIN,
    CONF_THRESHOLD,
    CONF_VALUE,
)

from . import ESP32TouchComponent, esp32_touch_ns, validate_touch_pad

DEPENDENCIES = ["esp32_touch", "esp32"]

CONF_ESP32_TOUCH_ID = "esp32_touch_id"
CONF_WAKEUP_THRESHOLD = "wakeup_threshold"
CONF_LOOKBACK_NUM_VALUES = "lookback_num_values"
CONF_SCAN_INTERVAL = "scan_interval"
CONF_MAX_DEVIATION = "max_deviation"
CONF_MAX_CONSECUTIVE_ANOMALIES = "max_consecutive_anomalies"

THRESHOLD_MODE_STATIC = "static"
THRESHOLD_MODE_DYNAMIC = "dynamic"

ESP32TouchBinarySensor = esp32_touch_ns.class_(
    "ESP32TouchBinarySensor", binary_sensor.BinarySensor
)

THRESHOLD_SCHEMA_STATIC = cv.Schema(
    {
        cv.Required(CONF_MODE): THRESHOLD_MODE_STATIC,
        cv.Required(CONF_VALUE): cv.uint32_t,
    }
)

THRESHOLD_SCHEMA_DYNAMIC = cv.Schema(
    {
        cv.Required(CONF_MODE): THRESHOLD_MODE_DYNAMIC,
        cv.Optional(CONF_INITIAL_VALUE, default=0): cv.uint32_t,
        cv.Optional(CONF_LOOKBACK_NUM_VALUES, default=5): cv.int_range(min=1),
        cv.Optional(
            CONF_SCAN_INTERVAL, default="1s"
        ): cv.positive_time_period_milliseconds,
        cv.Optional(CONF_MAX_DEVIATION, default="0.5%"): cv.percentage,
        cv.Optional(CONF_MAX_CONSECUTIVE_ANOMALIES, default=10): cv.positive_int,
    }
)


def _convert_int_to_threshold(config):
    """Convert legacy threshold value to new format."""
    try:
        threshold_value = cv.uint32_t(config)
        return {CONF_MODE: THRESHOLD_MODE_STATIC, CONF_VALUE: threshold_value}
    except cv.Invalid:
        return config


CONFIG_SCHEMA = binary_sensor.binary_sensor_schema(ESP32TouchBinarySensor).extend(
    {
        cv.GenerateID(CONF_ESP32_TOUCH_ID): cv.use_id(ESP32TouchComponent),
        cv.Required(CONF_PIN): validate_touch_pad,
        cv.Required(CONF_THRESHOLD): cv.All(
            _convert_int_to_threshold,
            cv.Any(
                THRESHOLD_SCHEMA_STATIC.schema,
                THRESHOLD_SCHEMA_DYNAMIC.schema,
            ),
        ),
        cv.Optional(CONF_WAKEUP_THRESHOLD, default=0): cv.uint32_t,
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_ESP32_TOUCH_ID])
    var = cg.new_Pvariable(
        config[CONF_ID],
        config[CONF_PIN],
        (
            config[CONF_THRESHOLD][CONF_VALUE]
            if config[CONF_THRESHOLD][CONF_MODE] == THRESHOLD_MODE_STATIC
            else config[CONF_THRESHOLD][CONF_INITIAL_VALUE]
        ),
        config[CONF_WAKEUP_THRESHOLD],
    )
    await binary_sensor.register_binary_sensor(var, config)
    cg.add(hub.register_touch_pad(var))

    if config[CONF_THRESHOLD][CONF_MODE] == THRESHOLD_MODE_DYNAMIC:
        cg.add(var.set_max_deviation(config[CONF_THRESHOLD][CONF_MAX_DEVIATION]))
        cg.add(
            var.set_max_consecutive_anomalies(
                config[CONF_THRESHOLD][CONF_MAX_CONSECUTIVE_ANOMALIES]
            )
        )
        cg.add(
            var.start_calibration(
                config[CONF_THRESHOLD][CONF_SCAN_INTERVAL],
                config[CONF_THRESHOLD][CONF_LOOKBACK_NUM_VALUES],
            )
        )
