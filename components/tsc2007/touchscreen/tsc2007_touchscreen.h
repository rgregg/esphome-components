#pragma once

#include "esphome/components/i2c/i2c.h"
#include "esphome/components/touchscreen/touchscreen.h"
#include "esphome/core/component.h"
#include "esphome/core/hal.h"


namespace esphome {
namespace tsc2007 {

/*!
  *    @brief  Different function commands
  */
typedef enum {
  MEASURE_TEMP0 = 0,
  MEASURE_AUX = 2,
  MEASURE_TEMP1 = 4,
  ACTIVATE_X = 8,
  ACTIVATE_Y = 9,
  ACTIVATE_YPLUS_X = 10,
  SETUP_COMMAND = 11,
  MEASURE_X = 12,
  MEASURE_Y = 13,
  MEASURE_Z1 = 14,
  MEASURE_Z2 = 15
} adafruit_tsc2007_function;

/*!
  *    @brief  Power and IRQ modes
  */
typedef enum {
  POWERDOWN_IRQON = 0,
  ADON_IRQOFF = 1,
  ADOFF_IRQON = 2,
} adafruit_tsc2007_power;

/*!
  *    @brief  ADC resolution
  */
typedef enum {
  ADC_12BIT = 0,
  ADC_8BIT = 1,
} adafruit_tsc2007_resolution;


class TSC2007Touchscreen : public touchscreen::Touchscreen, public i2c::I2CDevice {
 public:
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  void set_interrupt_pin(InternalGPIOPin *pin) { this->interrupt_pin_ = pin; }
  void set_reset_pin(GPIOPin *pin) { this->reset_pin_ = pin; }
  
 protected:
  void update_touches() override;
  uint16_t command(adafruit_tsc2007_function func, adafruit_tsc2007_power pwr, adafruit_tsc2007_resolution res);
  bool read_touch(uint16_t *x, uint16_t *y, uint16_t *z1, uint16_t *z2);

  InternalGPIOPin *interrupt_pin_{};
  GPIOPin *reset_pin_{};

};  


}  // namespace tsc2007
}  // namespace esphome