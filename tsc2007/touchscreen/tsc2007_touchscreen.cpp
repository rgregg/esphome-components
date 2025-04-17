#include "tsc2007_touchscreen.h"

#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

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
  // Initialize dimensions based on configuration
  if (this->x_max_ == 0) {
    this->x_max_ = this->display_->get_native_width();
  }
  if (this->y_max_ == 0) {
    this->y_max_ = this->display_->get_native_height();
  }
  ESP_LOGCONFIG(TAG, "TSC2007 Touchscreen setup complete.");
}

void TSC2007Touchscreen::dump_config() {
  ESP_LOGCONFIG(TAG, "TSC2007 Touchscreen:");
  ESP_LOGCONFIG(TAG, "  Calibration:");
  ESP_LOGCONFIG(TAG, "    X Min: %d", this->x_min_);
  ESP_LOGCONFIG(TAG, "    X Max: %d", this->x_max_);
  ESP_LOGCONFIG(TAG, "    Y Min: %d", this->y_min_);
  ESP_LOGCONFIG(TAG, "    Y Max: %d", this->y_max_);
  LOG_TOUCHSCREEN("  ", "Touchscreen");
}


void TSC2007Touchscreen::update_touches() {
  
}

void TSC2007Touchscreen::update() {
  // Check if it's time for a new reading
  const uint32_t now = millis();
  if (now - this->last_update_ < this->report_interval_) {
    return;
  }
  this->last_update_ = now;
  
  // Get touch point from the TSC2007
  TSPoint point = this->tsc2007_->get_point();
  
  // Check if touch is detected (non-zero pressure above threshold)
  if (point.z > this->threshold_) {
    // Map the raw touchscreen coordinates to display coordinates
    float x_norm = (point.x - this->x_min_) / float(this->x_max_ - this->x_min_);
    float y_norm = (point.y - this->y_min_) / float(this->y_max_ - this->y_min_);
    
    // Invert Y axis if needed (touchscreen usually has origin at top left)
    y_norm = 1.0f - y_norm;
    
    // Scale to display dimensions
    float x_pos = x_norm * this->display_width_;
    float y_pos = y_norm * this->display_height_;
    
    ESP_LOGV(TAG, "Touch: raw(%d, %d, %d) -> pos(%.1f, %.1f)", 
             point.x, point.y, point.z, x_pos, y_pos);
    
    // Send touch event
    this->touch(0, x_pos, y_pos, point.z);
    this->touch_detected_ = true;
  } else if (this->touch_detected_) {
    // Release touch if it was previously detected
    this->release(0);
    this->touch_detected_ = false;
  }
}

}  // namespace tsc2007
}  // namespace esphome