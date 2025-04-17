#include "tsc2007_touchscreen.h"

#include "esphome/core/log.h"
#include "esphome/core/helpers.h"


#define ERROR_CHECK(err) \
  if ((err) != i2c::ERROR_OK) { \
    this->status_set_warning("Failed to communicate"); \
    return 0; \
  }

namespace esphome {
namespace tsc2007 {

static const char *const TAG = "tsc2007.touchscreen";

void TSC2007Touchscreen::setup() {
  ESP_LOGCONFIG(TAG, "Setting up TSC2007 Touchscreen...");
  
  if (this->reset_pin_ != nullptr) {
    this->reset_pin_->setup();
    this->reset_pin_->digital_write(false);
    delay(5);
    this->reset_pin_->digital_write(true);
    delay(10);
  }
  if (this->interrupt_pin_ != nullptr) {
    this->interrupt_pin_->pin_mode(gpio::FLAG_INPUT);
    this->interrupt_pin_->setup();
    this->attach_interrupt_(this->interrupt_pin_, gpio::INTERRUPT_FALLING_EDGE);
  }

  if (this->x_raw_max_ == 0) {
    this->x_raw_max_ = this->display_->get_native_width();
  }
  if (this->y_raw_max_ == 0) {
    this->y_raw_max_ = this->display_->get_native_height();
  }

  this->command(MEASURE_TEMP0, POWERDOWN_IRQON, ADC_12BIT);

  ESP_LOGCONFIG(TAG, "TSC2007 Touchscreen setup complete.");
}

void TSC2007Touchscreen::dump_config() {
  ESP_LOGCONFIG(TAG, "TSC2007 Touchscreen:");
  LOG_I2C_DEVICE(this);
  LOG_PIN("  Interrupt Pin: ", this->interrupt_pin_);
  LOG_PIN("  Reset Pin: ", this->reset_pin_);
  ESP_LOGCONFIG(TAG, "  Width: %d", this->x_raw_max_);
  ESP_LOGCONFIG(TAG, "  Height: %d", this->y_raw_max_);
}

void TSC2007Touchscreen::update_touches() {
  
  uint16_t x, y, z1, z2;
  if (this->read_touch(&x, &y, &z1, &z2)) {
    ESP_LOGD(TAG, "Touch detected: x=%d, y=%d, z1=%d, z2=%d", x, y, z1, z2);
    this->add_raw_touch_position_(0, x, y, z1);
  }
  else {
    ESP_LOGD(TAG, "No touch detected");
  }
}

/*!
 *    @brief  Send a command and read 2 bytes from TSC
 *    @param  func The command function to make
 *    @param  pwr  The power mode to enter after command
 *    @param  res  The ADC resolution
 *    @return 12 bits of data shifted from the 16-bit read value
 */
uint16_t TSC2007Touchscreen::command(adafruit_tsc2007_function func,
                                 adafruit_tsc2007_power pwr,
                                 adafruit_tsc2007_resolution res) {

  i2c::ErrorCode err;
  uint8_t reply[2];

  uint8_t cmd = (uint8_t)func << 4;
  cmd |= (uint8_t)pwr << 2;
  cmd |= (uint8_t)res << 1;

  // send the command to the TSC
  err = this->write(&cmd, 1);
  ERROR_CHECK(err);
                          
  // Wait 1/2ms for conversion
  delayMicroseconds(500);

  err = this->read(reply, sizeof(reply));
  ERROR_CHECK(err);

  return ((uint16_t)reply[0] << 4) | (reply[1] >> 4); // 12 bits
}

/*!
 *    @brief  Read touch data from the TSC and then power down
 *    @param  x Pointer to 16-bit value we will store x reading
 *    @param  y Pointer to 16-bit value we will store y reading
 *    @param  z1 Pointer to 16-bit value we will store z1 pressure reading
 *    @param  z2 Pointer to 16-bit value we will store z2 pressure reading
 *    @return True if ADC was able to read the x & y values
 */
bool TSC2007Touchscreen::read_touch(uint16_t *x, uint16_t *y, uint16_t *z1,
                                  uint16_t *z2) {
  *z1 = command(MEASURE_Z1, ADON_IRQOFF, ADC_12BIT);
  *z2 = command(MEASURE_Z2, ADON_IRQOFF, ADC_12BIT);
  ESP_LOGVV(TAG, "read_touch: z1=%d, z2=%d", *z1, *z2);

  // take two measurements since there can be a 'flicker' on pen up

  uint16_t x1, y1, x2, y2;
  x1 = command(MEASURE_X, ADON_IRQOFF, ADC_12BIT);
  y1 = command(MEASURE_Y, ADON_IRQOFF, ADC_12BIT);
  x2 = command(MEASURE_X, ADON_IRQOFF, ADC_12BIT);
  y2 = command(MEASURE_Y, ADON_IRQOFF, ADC_12BIT);

  ESP_LOGVV(TAG, "read_touch: x1=%d, y1=%d, x2=%d, y2=%d", x1, y1, x2, y2);

  command(MEASURE_TEMP0, POWERDOWN_IRQON, ADC_12BIT);

  if (abs((int32_t)x1 - (int32_t)x2) > 100)
    ESP_LOGVV(TAG, "x1 and x2 differ too much");
    return false;
  if (abs((int32_t)y1 - (int32_t)y2) > 100)
    ESP_LOGVV(TAG, "y1 and y2 differ too much");
    return false;

  *x = x1;
  *y = y1;
  return (*x != 4095) && (*y != 4095);
}


}  // namespace tsc2007
}  // namespace esphome