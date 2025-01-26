#pragma once

#include "esphome/core/component.h"
#include "esphome/core/hal.h"
#include "esphome/components/sensor/sensor.h"

#include <cinttypes>

namespace esphome {
namespace hx711 {

enum HX711Gain : uint8_t {
  HX711_GAIN_128 = 1,
  HX711_GAIN_32 = 2,
  HX711_GAIN_64 = 3,
};

class HX711Sensor : public sensor::Sensor, public PollingComponent {
 public:
  void set_dout_pin(GPIOPin *dout_pin) { dout_pin_ = dout_pin; }
  void set_sck_pin(GPIOPin *sck_pin) { sck_pin_ = sck_pin; }
  void set_gain(HX711Gain gain) { gain_ = gain; }
  void set_settling_time(const uint16_t settling_time_ms) { this->settling_time_ms_ = settling_time_ms; }
  void set_power_down_after_reading(const bool power_down_after_reading) {
    this->power_down_after_reading_ = power_down_after_reading;
  }

  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;
  void update() override;
  void on_safe_shutdown() override;
  void on_shutdown() override;

  /// Powers up the HX711 by setting the PD_SCK pin to low (sets gain if needed) then timestamps the change.
  void power_up();
  /// Powers down the HX711 by setting the PD_SCK pin to high and waits 60µs then timestamps the change.
  void power_down();
  /// Returns true if the HX711 is powered down (PD_SCK pin is high).
  bool is_powered_down() const;

 protected:
  /// @brief Read sensor data from HX711.
  /// @param[out] result Pointer to store the read value.
  /// @param[in] force Force reading even if settling time has not elapsed.
  /// @return True if data was successfully read, false otherwise.
  bool read_sensor_(uint32_t *result, bool force = false);

  /// Powers up the HX711 by setting the PD_SCK pin to low.
  void power_up_internal_();
  /// Powers down the HX711 by setting the PD_SCK pin to high.
  void power_down_internal_();

  /// If true, HX711 is powered down after reading data.
  bool power_down_after_reading_{false};
  /// Settling time in milliseconds.
  /// Taken from the datasheet: "Settling time refers to the time from power up,
  /// reset, input channel change and gain change to valid stable output data."
  uint16_t settling_time_ms_{400};
  /// Timestamp (in millis() output) of the last change.
  /// Changed when read_sensor_() is called.
  uint32_t last_change_{0};

  GPIOPin *dout_pin_;
  GPIOPin *sck_pin_;
  /// Gain to set after new measurement.
  HX711Gain gain_{HX711_GAIN_128};
};

}  // namespace hx711
}  // namespace esphome
