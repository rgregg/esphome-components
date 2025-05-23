import esphome.codegen as cg

CODEOWNERS = ["@rgregg"]
DEPENDENCIES = ["display", "spi"]

thinkink_ns = cg.esphome_ns.namespace("thinkink")

cg.add_library(
    name="Adafruit EPD",
    repository="https://github.com/adafruit/Adafruit_EPD",
    version=None,
)