#include "adc_sensor.h"
#include "esphome/core/log.h"

namespace esphome {
namespace adc {

static const char *const TAG = "adc.common";

void ADCSensor::update() {
  float value_v = this->sample();
  ESP_LOGV(TAG, "'%s': Got voltage=%.4fV", this->get_name().c_str(), value_v);
  this->publish_state(value_v);
}

void ADCSensor::set_sample_count(uint8_t sample_count) {
  if (sample_count != 0) {
    this->sample_count_ = sample_count;
  }
}

void ADCSensor::set_sampling_mode(uint8_t sampling_mode) {
  if (sampling_mode >= 0 && sampling_mode <= 2) {
    this->sampling_mode_ = sampling_mode;
  }
}

float ADCSensor::get_setup_priority() const { return setup_priority::DATA; }

}  // namespace adc
}  // namespace esphome
