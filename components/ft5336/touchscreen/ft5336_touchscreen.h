#pragma once

#include "esphome/components/i2c/i2c.h"
#include "esphome/components/touchscreen/touchscreen.h"
#include "esphome/core/component.h"
#include "esphome/core/hal.h"


namespace esphome {
namespace ft5336 {

#define FT53XX_DEFAULT_ADDR 0x38 //!< I2C address
#define FT53XX_REG_VENDID 0xA3   //!< FocalTech's panel ID
#define FT53XX_REG_FIRMVERS 0xA6 //!< Firmware version
#define FT53XX_REG_CHIPID 0xA8   //!< Chip selecting

#define FT53XX_VENDID 0x11 //!< FocalTech's panel ID
#define FT5336_CHIPID 0x79 //!< Chip selecting

#define FT53XX_REG_NUMTOUCHES 0x02 //!< Number of touch points

#define FT5336_REG_THRESHGROUP 0x80 //!< Threshold for touch detection

#define FT5336_TD_STATUS 0x02 ///<! Touch status
#define FT5336_GEST_ID 0x01   ///<! Detected Gesture ID
#define FT5336_TOUCH1_XH 0x03 ///<! High byte of X touch #1
#define FT5336_TOUCH1_XL 0x04 ///<! Low byte of X touch #1
#define FT5336_TOUCH1_YH 0x05 ///<! High byte of Y touch #1
#define FT5336_TOUCH1_YL 0x06 ///<! Low byte of Y touch #1

#define FT53XX_NUM_X 0x33 //!< Touch X position
#define FT53XX_NUM_Y 0x34 //!< Touch Y position

#define FT53XX_REG_MODE 0x00        //!< Device mode, either WORKING or FACTORY
#define FT53XX_REG_CALIBRATE 0x02   //!< Calibrate mode
#define FT53XX_REG_WORKMODE 0x00    //!< Work mode
#define FT53XX_REG_FACTORYMODE 0x40 //!< Factory mode
#define FT53XX_REG_THRESHHOLD 0x80  //!< Threshold for touch detection
#define FT53XX_REG_POINTRATE 0x88   //!< Point rate

// calibrated for Adafruit 3.5" ctp screen
#define FT62XX_DEFAULT_THRESHOLD 128 //!< Default threshold for touch detection


class FT5336Touchscreen : public touchscreen::Touchscreen, public i2c::I2CDevice {
 public:
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  void set_interrupt_pin(InternalGPIOPin *pin) { this->interrupt_pin_ = pin; }
  void set_reset_pin(GPIOPin *pin) { this->reset_pin_ = pin; }
  
 protected:
  void update_touches() override;
  uint8_t touched(void);

  InternalGPIOPin *interrupt_pin_{};
  GPIOPin *reset_pin_{};

private:
    void writeRegister8(uint8_t reg, uint8_t val);
    uint8_t readRegister8(uint8_t reg);

    void readData(void);
    uint8_t touches, gesture;
    uint16_t touchX[5], touchY[5], touchID[5];
};  


}  // namespace ft5336
}  // namespace esphome