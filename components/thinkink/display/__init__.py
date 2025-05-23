from esphome.codegen import cg
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_MODEL, CONF_WIDTH, CONF_HEIGHT, CONF_DC_PIN, CONF_RESET_PIN



ThinkInkDisplay = thinkink_ns.class_(
    "ThinkInkDisplay", cg.PollingComponent, display.Display, spi.SPIComponent
)

ThinkInkDisplayModel = thinkink_ns.enum("ThinkInkDisplayModel")
MODELS = {
    "154_Tricolor_RW": ThinkInkDisplayModel.THINKINK_154_TRICOLOR_RW,
    "154_Tricolor_Z17": ThinkInkDisplayModel.THINKINK_154_TRICOLOR_Z17,
    "154_Tricolor_Z90": ThinkInkDisplayModel.THINKINK_154_TRICOLOR_Z90,
    "213_Tricolor_RW": ThinkInkDisplayModel.THINKINK_213_TRICOLOR_RW,
    "213_Tricolor_Z16": ThinkInkDisplayModel.THINKINK_213_TRICOLOR_Z16,
    "270_Tricolor_C44": ThinkInkDisplayModel.THINKINK_270_TRICOLOR_C44,
    "270_Tricolor_Z70": ThinkInkDisplayModel.THINKINK_270_TRICOLOR_Z70,
    "290_Tricolor_RH": ThinkInkDisplayModel.THINKINK_290_TRICOLOR_RH,
    "290_Tricolor_Z10": ThinkInkDisplayModel.THINKINK_290_TRICOLOR_Z10,
    "290_Tricolor_Z13": ThinkInkDisplayModel.THINKINK_290_TRICOLOR_Z13,
    "290_Tricolor_Z94": ThinkInkDisplayModel.THINKINK_290_TRICOLOR_Z94,
    "420_Tricolor_RW": ThinkInkDisplayModel.THINKINK_420_TRICOLOR_RW,
    "420_Tricolor_Z21": ThinkInkDisplayModel.THINKINK_420_TRICOLOR_Z21,
    "154_Grayscale4_T8": ThinkInkDisplayModel.THINKINK_154_GRAYSCALE4_T8,
    "213_Grayscale4_T5": ThinkInkDisplayModel.THINKINK_213_GRAYSCALE4_T5,
    "270_Grayscale4_W3": ThinkInkDisplayModel.THINKINK_270_GRAYSCALE4_W3,
    "290_Grayscale4_T5": ThinkInkDisplayModel.THINKINK_290_GRAYSCALE4_T5,
    "420_Grayscale4_T2": ThinkInkDisplayModel.THINKINK_420_GRAYSCALE4_T2,
    "154_Mono_D27": ThinkInkDisplayModel.THINKINK_154_MONO_D27,
    "154_Mono_D67": ThinkInkDisplayModel.THINKINK_154_MONO_D67,
    "154_Mono_M10": ThinkInkDisplayModel.THINKINK_154_MONO_M10,
    "213_Mono_B72": ThinkInkDisplayModel.THINKINK_213_MONO_B72,
    "213_Mono_B73": ThinkInkDisplayModel.THINKINK_213_MONO_B73,
    "213_Mono_BN": ThinkInkDisplayModel.THINKINK_213_MONO_BN,
    "213_Mono_GDEY0213B74": ThinkInkDisplayModel.THINKINK_213_MONO_GDEY0213B74,
    "213_Mono_M21": ThinkInkDisplayModel.THINKINK_213_MONO_M21,
    "290_Mono_BN": ThinkInkDisplayModel.THINKINK_290_MONO_BN,
    "290_Mono_M06": ThinkInkDisplayModel.THINKINK_290_MONO_M06,
    "420_Mono_BN": ThinkInkDisplayModel.THINKINK_420_MONO_BN,
    "420_Mono_M06": ThinkInkDisplayModel.THINKINK_420_MONO_M06,
}


CONFIG_SCHEMA = (
    display.FULL_DISPLAY_SCHEMA.extend(
        {
            cv.GenerateID(): cv.declare_id(ThinkInkDisplay),
            cv.Required(CONF_MODEL): cv.one_of(*MODELS, lower=True),
            cv.Optional(CONF_WIDTH): cv.positive_int,
            cv.Optional(CONF_HEIGHT): cv.positive_int,
            cv.Optional(CONF_RESET_PIN): cv.gpio_output_pin_schema,
            cv.Optional(CONF_DC_PIN): cv.gpio_output_pin_schema,
        }
    ).extend(spi.spi_device_schema()),
    # Additional validation logic (e.g. per model) can be added here
)

async def to_code(config):

    model = MODELS[config[CONF_MODEL]]


    var = cg.new_Pvariable(config[CONF_ID])
    await display.register_display(var, config)
    await spi.register_spi_device(var, config)

    if CONF_TYPE in config:
        cg.add(var.set_model(config[CONF_TYPE]))

    if CONF_WIDTH in config:
        cg.add(var.set_width(config[CONF_WIDTH]))

    if CONF_HEIGHT in config:
        cg.add(var.set_height(config[CONF_HEIGHT]))

    if CONF_RESET_PIN in config:
        cg.add(var.set_reset_pin(await cg.gpio_pin_expression(config[CONF_RESET_PIN])))

    if CONF_DC_PIN in config:
        cg.add(var.set_dc_pin(await cg.gpio_pin_expression(config[CONF_DC_PIN])))