from esphome import pins
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

TSC2007_I2CADDR_DEFAULT = 0x48
TSC2007_DEFAULT_TOUCH_TIMEOUT = "50ms"

CONFIG_SCHEMA = (
    touchscreen.touchscreen_schema(TSC2007_DEFAULT_TOUCH_TIMEOUT)
    .extend(
        {
            cv.GenerateID(): cv.declare_id(TSC2007Touchscreen),
            cv.Optional(CONF_INTERRUPT_PIN): pins.internal_gpio_input_pin_schema,
            cv.Optional(CONF_RESET_PIN): pins.gpio_output_pin_schema,
        }
    )
    .extend(i2c.i2c_device_schema(TSC2007_I2CADDR_DEFAULT))
)

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await touchscreen.register_touchscreen(var, config)
    await i2c.register_i2c_device(var, config)
    
    if interrupt_pin := config.get(CONF_INTERRUPT_PIN):
        cg.add(var.set_interrupt_pin(await cg.gpio_pin_expression(interrupt_pin)))
    if reset_pin := config.get(CONF_RESET_PIN):
        cg.add(var.set_reset_pin(await cg.gpio_pin_expression(reset_pin)))
