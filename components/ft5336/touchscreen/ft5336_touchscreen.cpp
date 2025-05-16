/*!
 * @file ft5336_touchscreen.cpp
 *
 * @mainpage Adafruit FT5336 Library
 *
 * @section intro_sec Introduction
 *
 * This is a library for the Adafruit Capacitive Touch Screens
 *
 * ----> http://www.adafruit.com/products/1947
 *
 * Check out the links above for our tutorials and wiring diagrams
 * This chipset uses I2C to communicate
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 * @section author Author
 *
 * Written by Limor Fried/Ladyada for Adafruit Industries.
 *
 * @section license License

 * MIT license, all text above must be included in any redistribution
 */

#include "ft5336_touchscreen.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace ft5336 {
static const char *const TAG = "ft5336.touchscreen";

void FT5336Touchscreen::setup() {
  ESP_LOGCONFIG(TAG, "Setting up FT5336 Touchscreen...");
  
  // default config
  this->x_raw_min_ = 0;
  this->y_raw_min_ = 0;
  this->x_raw_max_ = this->display_->get_native_width();
  this->y_raw_max_ = this->display_->get_native_height();

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

  // change threshhold to be higher/lower
  // writeRegister8(FT53XX_REG_THRESHHOLD, thresh);

  uint8_t vendid = this->readRegister8(FT53XX_REG_VENDID);
  ESP_LOGD(TAG, "Vendor ID: 0x%02X", vendid);

  if (vendid != FT53XX_VENDID) {
    ESP_LOGE(TAG, "Vendor ID mismatch");
    this->mark_failed();
    return;
  }

  uint8_t chipid = this->readRegister8(FT53XX_REG_CHIPID);
  ESP_LOGD(TAG, "Chip ID: 0x%02X", chipid);
  if (chipid != FT5336_CHIPID) {
    ESP_LOGE(TAG, "Chip ID mismatch");
    this->mark_failed();
    return;
  }

  ESP_LOGCONFIG(TAG, "FT5336 Touchscreen setup complete.");
}

uint8_t FT5336Touchscreen::readRegister8(uint8_t reg) {
    uint8_t buffer[1];
    i2c::ErrorCode err = this->read_register(reg, buffer, 1); 

    if (err != i2c::ERROR_OK) {
        ESP_LOGE(TAG, "Failed to read register 0x%02X: %d", reg, err);
        return 0;
    }
    ESP_LOGD(TAG, "Read register 0x%02X: 0x%02X", reg, buffer[0]);
    return buffer[0];
}
  
void FT5336Touchscreen::writeRegister8(uint8_t reg, uint8_t val) {
    uint8_t buffer[1] = { val };
    i2c::ErrorCode err = this->write_register(reg, buffer, 1);
    if (err != i2c::ERROR_OK) {
        ESP_LOGE(TAG, "Failed to write register 0x%02X: %d", reg, err);
        return;
    }
    ESP_LOGD(TAG, "Write register 0x%02X: 0x%02X", reg, val);
}

void FT5336Touchscreen::dump_config() {
    ESP_LOGCONFIG(TAG, "FT5336 Touchscreen:");
    LOG_I2C_DEVICE(this);
    LOG_PIN("  Interrupt Pin: ", this->interrupt_pin_);
    LOG_PIN("  Reset Pin: ", this->reset_pin_);
    ESP_LOGCONFIG(TAG, "  Width: %d", this->x_raw_max_);
    ESP_LOGCONFIG(TAG, "  Height: %d", this->y_raw_max_);

#ifdef FT5336_DEBUG
    ESP_LOGD(TAG, "  Debugging enabled");
    ESP_LOGD(TAG, "  Vend ID: 0x%02X", this->readRegister8(FT53XX_REG_VENDID));
    ESP_LOGD(TAG, "  Chip ID: 0x%02X", this->readRegister8(FT53XX_REG_CHIPID));
    ESP_LOGD(TAG, "  Firm V: %02X", this->readRegister8(FT53XX_REG_FIRMVERS));
    ESP_LOGD(TAG, "  Thresh: %02X", this->readRegister8(FT5336_REG_THRESHGROUP));
    // dump all registers
    for (int16_t i = 0; i < 0x10; i++) {
        ESP_LOGD(TAG, "I2C $%02X = 0x%02X", i, this->readRegister8(i));
    }
#endif
}
void FT5336Touchscreen::update_touches() {
    uint8_t i2cdat[32];
    uint8_t addr = 0;
    i2c::ErrorCode err = this->read_register(addr, i2cdat, 32);
    if (err != i2c::ERROR_OK) {
        ESP_LOGE(TAG, "Failed to read data: %d", err);
        return;
    }

    touches = i2cdat[FT5336_TD_STATUS];
    if ((touches > 5) || (touches == 0)) {
      touches = 0;
    }
  
    for (uint8_t i = 0; i < touches; i++) {
      touchX[i] = i2cdat[FT5336_TOUCH1_XH + i * 6] & 0x0F;
      touchX[i] <<= 8;
      touchX[i] |= i2cdat[FT5336_TOUCH1_XL + i * 6];
  
      touchY[i] = i2cdat[FT5336_TOUCH1_YH + i * 6] & 0x0F;
      touchY[i] <<= 8;
      touchY[i] |= i2cdat[FT5336_TOUCH1_YL + i * 6];
  
      touchID[i] = i2cdat[FT5336_TOUCH1_YH + i * 6] >> 4;

      this->add_raw_touch_position_(touchID[i], touchX[i], touchY[i], 1);
      ESP_LOGD(TAG, "ID #%d\t(%d, %d)", touchID[i], touchX[i], touchY[i]);
    }
  }

/**************************************************************************/
/*!
    @brief  Determines if there are any touches detected
    @returns Number of touches detected, can be 0 or 1-5 for multitouch
*/
/**************************************************************************/
uint8_t FT5336Touchscreen::touched(void) {
  uint8_t n = readRegister8(FT53XX_REG_NUMTOUCHES);
  if (n > 5) {
    n = 0;
  }
  return n;
}

} // namespace ft5336
} // namespace esphome

