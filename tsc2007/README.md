# ESPHome TSC2007 Touchscreen Component

This component provides support for resistive touch screens using the TSC2007 touch controller.

## Installation

1. Copy the `components` directory to your ESPHome configuration directory.
2. Add the following to your ESPHome configuration YAML:

```yaml
# Example configuration
external_components:
  - source: 
      type: local
      path: components

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
    address: 0x48
    display: my_display
    
    on_touch:
      then:
        - logger.log:
            format: "Touch at (%d, %d)"
            args: [touch.x, touch.y]
    
    on_release:
      then:
        - logger.log: "Touch Released"
```

## Calibration

For the best results, you should calibrate your touchscreen. You'll need to find the raw X/Y coordinates at the corners of your screen.

1. Set up the component with default values
2. Add debug logging to see the raw values when touching corners
3. Use these values to set the calibration_* values in your configuration

## Example: Using with display pages to show touch position

```yaml
# Create a display page to show touch data
display_page:
  - id: page1
    lambda: |-
      it.rectangle(0, 0, it.get_width(), it.get_height(), COLOR_WHITE);
      it.print(10, 10, id(font1), COLOR_BLACK, TextAlign::TOP_LEFT, 
               "Touch Detected!");
      it.printf(10, 40, id(font1), COLOR_BLACK, TextAlign::TOP_LEFT,
                "X: %.1f", x);
      it.printf(10, 70, id(font1), COLOR_BLACK, TextAlign::TOP_LEFT,
                "Y: %.1f", y);
      it.printf(10, 100, id(font1), COLOR_BLACK, TextAlign::TOP_LEFT,
                "Pressure: %.1f", pressure);

tsc2007:
  id: my_touchscreen
  display: my_display
  
  on_touch:
    then:
      - display.page.show: page1
```