from esphonme import pins
import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import i2c, touchscreen
from esphome.const import CONF_ID, CONF_INTERRUPT_PIN, CONF_RESET_PIN

from .. import tsc2007_ns

DEPENDENCIES = ["i2c"]

TSC2007Touchscreen = tsc2007_ns.class_(
    "TSC2007Touchscreen", 
    touchscreen.Touchscreen,
    i2c.I2CDevice,
)

# CONF_REPORT_INTERVAL = "report_interval"
CONF_CALIBRATION_X_MIN = "calibration_x_min"
CONF_CALIBRATION_X_MAX = "calibration_x_max"
CONF_CALIBRATION_Y_MIN = "calibration_y_min"
CONF_CALIBRATION_Y_MAX = "calibration_y_max"

CONFIG_SCHEMA = (
    touchscreen.touchscreen_schema("50ms")
    .extend(
        {
            cv.GenerateID(): cv.declare_id(TSC2007Touchscreen),
            cv.Optional(CONF_CALIBRATION_X_MIN, default=0): cv.int_,
            cv.Optional(CONF_CALIBRATION_X_MAX, default=4095): cv.int_,
            cv.Optional(CONF_CALIBRATION_Y_MIN, default=0): cv.int_,
            cv.Optional(CONF_CALIBRATION_Y_MAX, default=4095): cv.int_,
            cv.Optional(CONF_INTERRUPT_PIN): pins.internal_gpio_input_pin_schema,
            cv.Optional(CONF_RESET_PIN): pins.gpio_output_pin_schema,
        }
    )
    .extend(i2c.i2c_device_schema(0x48))
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await touchscreen.register_touchscreen(var, config)
    await i2c.register_i2c_device(var, config)


    if calibration_x_min := config.get(CONF_CALIBRATION_X_MIN):
        cg.add(var.set_calibration_x_min(calibration_x_min))
    if calibration_x_max := config.get(CONF_CALIBRATION_X_MAX):
        cg.add(var.set_calibration_x_max(calibration_x_max))
    if calibration_y_min := config.get(CONF_CALIBRATION_Y_MIN):
        cg.add(var.set_calibration_y_min(calibration_y_min))
    if calibration_y_max := config.get(CONF_CALIBRATION_Y_MAX):
        cg.add(var.set_calibration_y_max(calibration_y_max))

    if interrupt_pin := config.get(CONF_INTERRUPT_PIN):
        cg.add(var.set_interrupt_pin(await cg.gpio_pin_expression(interrupt_pin)))
    if reset_pin := config.get(CONF_RESET_PIN):
        cg.add(var.set_reset_pin(await cg.gpio_pin_expression(reset_pin)))