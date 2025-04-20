# ESPHome TSC2007 Touchscreen Component

This component provides support for resistive touch screens using the TSC2007 touch controller.

This code is based on the [Adafruit TSC2007 driver](https://github.com/adafruit/Adafruit_TSC2007) for Arduino. 

Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing products from [Adafruit](https://adafruit.com)!

## Installation

1. Add the following to your ESPHome configuration YAML:

```yaml
# Example configuration
external_components:
  - source: 
      url: github://rgregg/esphome-components.git@main
    components:
      - tsc2007
      

i2c:
  sda: GPIO21
  scl: GPIO22
  scan: true
  frequency: 400kHz

display:
  - platform: ...  # Your display type here
    # display configuration...
    id: my_display

# TSC2007 touchscreen configuration
touchscreen:
  - platform: tsc2007
    id: my_touchscreen
    display: my_display
    address: 0x48 # Default address for the TSC2007
    calibration:  # Use values based on the calibration instructions below (optional)
      x_min: 366  
      x_max: 3649
      y_min: 208
      y_max: 3808
    transform:   # Use values based on your display configuration (optional)
      mirror_x: false
      mirror_y: false
      swap_xy: false
    
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

## Calibration

For the best results, you should calibrate your touchscreen. You'll need to find the raw X/Y coordinates at the corners of your screen.

1. Set up the component with default values
2. Add debug logging to see the raw values when touching corners
3. Use these values to set the calibration_* values in your configuration
