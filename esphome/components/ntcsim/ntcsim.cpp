#include "ntcsim.h"

#include "esphome/core/log.h"

namespace esphome {
namespace ntcsim {

static const char *const TAG = "ntcsimulation";

void NTCSimulation::setup() {
  sensor_->add_on_state_callback([this](float value) { process_(value); });
  if (sensor_->has_state())
    process_(sensor_->state);
}
void NTCSimulation::dump_config() {
  LOG_SENSOR("", "NTC Simulation", this);
  ESP_LOGI(TAG, "  Coefficient a: %.8f", a_);
  ESP_LOGI(TAG, "  Coefficient b: %.8f", b_);
  ESP_LOGI(TAG, "  Coefficient c: %.8f", c_);
}

float NTCSimulation::get_setup_priority() const { return setup_priority::DATA; }

void NTCSimulation::process_(float value) {
  if (std::isnan(value)) {
    publish_state(NAN);
    return;
  }
  double k = 273.15;
  // https://de.wikipedia.org/wiki/Steinhart-Hart-Gleichung#cite_note-stein2_s4-3
  double t = value + k;
  double y = (a_ - 1 / (t)) / (2 * c_);
  double x = sqrt(pow(b_ / (3 * c_), 3) + y * y);
  double resistance = exp(pow(x - y, 1 / 3.0) - pow(x + y, 1 / 3.0));
  ESP_LOGI(TAG, "got resistor value %f for temperature %f", resistance, value);
  ESP_LOGD(TAG, "'%s' - Resistance: %.1f Ohm", this->name_.c_str(), resistance);
  publish_state(resistance);
#ifdef USE_OUTPUT
  if (output_) {
    double normalized_resistance = resistance / resistance_;
    ESP_LOGI(TAG, "send normalized resistor value %f to output device", normalized_resistance);
    output_->set_level(normalized_resistance);
  }
#endif
}

}  // namespace ntcsim
}  // namespace esphome
