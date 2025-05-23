#pragma once

#include "esphome/core/component.h"
#include "esphome/components/spi/spi.h"
#include "esphome/components/display/display_buffer.h"

namespace esphome {
namespace thinkink {

enum ThinkInkDisplayModel {
    THINKINK_154_TRICOLOR_RW = 0,
    THINKINK_154_TRICOLOR_Z17,
    THINKINK_154_TRICOLOR_Z90,
    THINKINK_213_TRICOLOR_RW,
    THINKINK_213_TRICOLOR_Z16,
    THINKINK_270_TRICOLOR_C44,
    THINKINK_270_TRICOLOR_Z70,
    THINKINK_290_TRICOLOR_RH,
    THINKINK_290_TRICOLOR_Z10,
    THINKINK_290_TRICOLOR_Z13,
    THINKINK_290_TRICOLOR_Z94,
    THINKINK_420_TRICOLOR_RW,
    THINKINK_420_TRICOLOR_Z21,
    THINKINK_154_GRAYSCALE4_T8,
    THINKINK_213_GRAYSCALE4_T5,
    THINKINK_270_GRAYSCALE4_W3,
    THINKINK_290_GRAYSCALE4_T5,
    THINKINK_420_GRAYSCALE4_T2,
    THINKINK_154_MONO_D27,
    THINKINK_154_MONO_D67,
    THINKINK_154_MONO_M10,
    THINKINK_213_MONO_B72,
    THINKINK_213_MONO_B73,
    THINKINK_213_MONO_BN,
    THINKINK_213_MONO_GDEY0213B74,
    THINKINK_213_MONO_M21,
    THINKINK_290_MONO_BN,
    THINKINK_290_MONO_M06,
    THINKINK_420_MONO_BN,
    THINKINK_420_MONO_M06,
};

class ThinkInkDisplay : public display::DisplayBuffer, 
                        public Component, 
                        public spi::SPIDevice {
    
    float get_setup_priority() const override;
    void set_dc_pin(GPIOPin *dc_pin) { dc_pin_ = dc_pin; }
    void set_reset_pin(GPIOPin *reset) { this->reset_pin_ = reset; }
    void set_busy_pin(GPIOPin *busy) { this->busy_pin_ = busy; }
    void set_reset_duration(uint32_t reset_duration) { this->reset_duration_ = reset_duration; }
    
    void command(uint8_t value);
    void data(uint8_t value);
    void cmd_data(const uint8_t *data, size_t length);
    
    virtual void display() = 0;
    virtual void initialize() = 0;
    virtual void deep_sleep() = 0;
    
    void update() override;
    
    void setup() override;
    
    void on_safe_shutdown() override;
    
    protected:
    bool wait_until_idle_();
    
    void setup_pins_();
    
    void reset_() {
        if (this->reset_pin_ != nullptr) {
        this->reset_pin_->digital_write(false);
        delay(reset_duration_);  // NOLINT
        this->reset_pin_->digital_write(true);
        delay(20);
        }
    }
    
    virtual int get_width_controller() { return this->get_width_internal(); };
    
    virtual uint32_t get_buffer_length_() = 0;  // NOLINT(readability-identifier-naming)
    uint32_t reset_duration_{200};
    
    void start_command_();
    void end_command_();
    void start_data_();
    void end_data_();
    
    GPIOPin *reset_pin_{nullptr};
    GPIOPin *dc_pin_;
    GPIOPin *busy_pin_{nullptr};
    virtual uint32_t idle_timeout_() { return 1000u; }  // NOLINT(readability-identifier-naming)
                          
                        }

}  // namespace thinkink
}