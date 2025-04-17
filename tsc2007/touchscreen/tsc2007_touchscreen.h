#pragma once

#include "esphomne/components/i2c/i2c.h"
#include "esphome/components/touchscreen/touchscreen.h"
#include "esphome/core/component.h"
#include "esphome/core/hal.h"

namespace esphome {
namespace tsc2007 {

class TSC2007Touchscreen : public touchscreen::Touchscreen, public i2c::I2CDevice {
 public:
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  void set_calibration_x_min(int16_t value) { x_min_ = value; }
  void set_calibration_x_max(int16_t value) { x_max_ = value; }
  void set_calibration_y_min(int16_t value) { y_min_ = value; }
  void set_calibration_y_max(int16_t value) { y_max_ = value; }

  void set_interrupt_pin(InternalGPIOPin *pin) { this->pin_interrupt_ = pin; }
  void set_reset_pin(GPIOPin *pin) { this->reset_pin_ = pin; }
  
 protected:  
  int16_t x_min_{0};
  int16_t x_max_{4095};
  int16_t y_min_{0};
  int16_t y_max_{4095};

  InternalGPIOPin **interrupt_pin_{};
  GPIOPin *reset_pin_{};

  // Last processed state
  uint32_t last_update_{0};
  bool touch_detected_{false};
};

}  // namespace tsc2007
}  // namespace esphome