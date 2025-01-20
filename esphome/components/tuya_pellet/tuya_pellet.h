#pragma once

#include "esphome/core/component.h"
#include "esphome/components/tuya/tuya.h"
#include "esphome/components/climate/climate.h"

namespace esphome {
namespace tuya {

class TuyaPellet : public climate::Climate, public Component {
 public:
  void setup() override;
  void loop() override;
  void dump_config() override;
  void set_supports_heat(bool supports_heat) { this->supports_heat_ = supports_heat; }
  void set_switch_id(uint8_t switch_id) { this->switch_id_ = switch_id; }
  void set_active_state_id(uint8_t state_id) { this->active_state_id_ = state_id; }
  void set_active_state_heating_value(uint8_t value) { this->active_state_heating_value_ = value; }
  void set_eco_mode_id(uint8_t eco_mode_id) {this->eco_mode_id_ = eco_mode_id; }
  void set_e1_value(uint8_t e1_value) { this->e1_value_ = e1_value; }
  void set_e2_value(uint8_t e2_value) { this->e2_value_ = e2_value; }
  void set_heat_level_id(uint8_t power_mode_id) {this->power_mode_id_ = power_mode_id; }
  void set_p1_value(uint8_t p1_value) { this->p1_value_ = p1_value; }
  void set_p2_value(uint8_t p2_value) { this->p2_value_ = p2_value; }
  void set_p3_value(uint8_t p3_value) { this->p3_value_ = p3_value; }
  void set_p4_value(uint8_t p4_value) { this->p4_value_ = p4_value; }
  void set_target_temperature_id(uint8_t target_temperature_id) {
    this->target_temperature_id_ = target_temperature_id;
  }
  void set_current_temperature_id(uint8_t current_temperature_id) {
    this->current_temperature_id_ = current_temperature_id;
  }
  void set_current_temperature_multiplier(float temperature_multiplier) {
    this->current_temperature_multiplier_ = temperature_multiplier;
  }
  void set_target_temperature_multiplier(float temperature_multiplier) {
    this->target_temperature_multiplier_ = temperature_multiplier;
  }

  void set_reports_fahrenheit() { this->reports_fahrenheit_ = true; }

  void set_tuya_parent(Tuya *parent) { this->parent_ = parent; }

 protected:
  /// Override control to change settings of the climate device.
  void control(const climate::ClimateCall &call) override;

  /// Override control to change settings of eco mode.
  void control_eco_mode_(const climate::ClimateCall &call);

  /// Override control to change settings of power mode.
  void control_heat_level_(const climate::ClimateCall &call);

  /// Return the traits of this controller.
  climate::ClimateTraits traits() override;
  tuya_pellet::PelletTraits p_traits() override;

  /// Re-compute the target temperature of this climate controller.
  void compute_target_temperature_();

  /// Re-compute the state of this climate controller.
  void compute_state_();

  /// Switch the climate device to the given climate mode.
  void switch_to_action_(climate::ClimateAction action);

  Tuya *parent_;
  bool supports_heat_;
  optional<uint8_t> switch_id_{};
  optional<uint8_t> active_state_id_{};
  optional<uint8_t> active_state_heating_value_{};
  optional<uint8_t> target_temperature_id_{};
  optional<uint8_t> current_temperature_id_{};
  float current_temperature_multiplier_{1.0f};
  float target_temperature_multiplier_{1.0f};
  float hysteresis_{1.0f};
  uint8_t active_state_;
  uint8_t eco_mode_id_;
  uint8_t e1_value_{0};
  uint8_t e2_value_{1};
  uint8_t heat_level_id_;
  uint8_t p1_value_{0};
  uint8_t p2_value_{1};
  uint8_t p3_value_{2};
  uint8_t p4_value_{3};
  bool heating_state_{false};
  float manual_temperature_;
  bool reports_fahrenheit_{false};
};

}  // namespace tuya
}  // namespace esphome
