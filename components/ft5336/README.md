# ESPHome FT5336 Touchscreen Component

This component provides support for capacitive touch screens using the FT5336 touch controller.
This controller is featured on the Adafruit 3.5" Capacitive Touch TFT Feather Wing.

This code is based on the [Adafruit FT5336 driver](https://github.com/adafruit/Adafruit_FT5336) for Arduino. 

Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing products from [Adafruit](https://adafruit.com)!

## Installation

1. Add the following to your ESPHome configuration YAML:

```yaml
# Example configuration
external_components:
  - source: 
      url: github://rgregg/esphome-components.git@dev
    components:
      - ft5336
      

i2c:
  sda: ..
  scl: ..

display:
  - platform: ...  # Your display type here
    # display configuration...
    id: my_display

# TSC2007 touchscreen configuration
touchscreen:
  - platform: ft5336
    id: my_touchscreen
    display: my_display
    address: 0x38 # Default address for the RT5336
    
    on_touch:
      - lambda: |-
          ESP_LOGI("cal", "x=%d, y=%d, x_raw=%d, y_raw=%0d",
              touch.x,
              touch.y,
              touch.x_raw,
              touch.y_raw
              );    
    on_release:
      then:
        - logger.log: "Touch Released"
```

