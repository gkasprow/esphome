#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/core/automation.h"
#include "esphome/components/output/float_output.h"

#ifdef USE_ESP32

// Forward declaration
struct dac_oneshot_s;
typedef struct dac_oneshot_s *dac_oneshot_handle_t;

namespace esphome {
namespace esp32_dac {

class ESP32DAC : public output::FloatOutput, public Component {
 public:
  void set_pin(InternalGPIOPin *pin) { pin_ = pin; }

  /// Initialize pin
  void setup() override;
  void on_safe_shutdown() override;
  void dump_config() override;
  /// HARDWARE setup_priority
  float get_setup_priority() const override { return setup_priority::HARDWARE; }

 protected:
  void write_state(float state) override;

  InternalGPIOPin *pin_;
#ifdef USE_ESP_IDF
  dac_oneshot_handle_t dac_handle_;
#endif
};

}  // namespace esp32_dac
}  // namespace esphome

#endif
