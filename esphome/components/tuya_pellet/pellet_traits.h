#pragma once

#include "esphome/core/helpers.h"
#include "pellet_mode.h"
#include <set>

namespace esphome {
namespace climate {

/** This class contains all static data for climate devices.
 *
 * All climate devices must support these features:
 *  - OFF mode
 *  - Target Temperature
 *
 * All other properties and modes are optional and the integration must mark
 * each of them as supported by setting the appropriate flag here.
 *
 *  - supports current temperature - if the climate device supports reporting a current temperature
 *  - supports two point target temperature - if the climate device's target temperature should be
 *     split in target_temperature_low and target_temperature_high instead of just the single target_temperature
 *  - supports modes:
 *    - auto mode (automatic control)
 *    - cool mode (lowers current temperature)
 *    - heat mode (increases current temperature)
 *    - dry mode (removes humidity from air)
 *    - fan mode (only turns on fan)
 *  - supports action - if the climate device supports reporting the active
 *    current action of the device with the action property.
 *  - supports fan modes - optionally, if it has a fan which can be configured in different ways:
 *    - on, off, auto, high, medium, low, middle, focus, diffuse, quiet
 *  - supports swing modes - optionally, if it has a swing which can be configured in different ways:
 *    - off, both, vertical, horizontal
 *
 * This class also contains static data for the climate device display:
 *  - visual min/max temperature - tells the frontend what range of temperatures the climate device
 *     should display (gauge min/max values)
 *  - temperature step - the step with which to increase/decrease target temperature.
 *     This also affects with how many decimal places the temperature is shown
 */
class PelletTraits {
 public:
  bool get_supports_current_temperature() const { return supports_current_temperature_; }
  void set_supports_current_temperature(bool supports_current_temperature) {
    supports_current_temperature_ = supports_current_temperature;
  }
  bool get_supports_current_humidity() const { return supports_current_humidity_; }
  void set_supports_current_humidity(bool supports_current_humidity) {
    supports_current_humidity_ = supports_current_humidity;
  }
  bool get_supports_two_point_target_temperature() const { return supports_two_point_target_temperature_; }
  void set_supports_two_point_target_temperature(bool supports_two_point_target_temperature) {
    supports_two_point_target_temperature_ = supports_two_point_target_temperature;
  }

  void set_supports_action(bool supports_action) { supports_action_ = supports_action; }
  bool get_supports_action() const { return supports_action_; }

  float get_visual_min_temperature() const { return visual_min_temperature_; }
  void set_visual_min_temperature(float visual_min_temperature) { visual_min_temperature_ = visual_min_temperature; }
  float get_visual_max_temperature() const { return visual_max_temperature_; }
  void set_visual_max_temperature(float visual_max_temperature) { visual_max_temperature_ = visual_max_temperature; }
  float get_visual_target_temperature_step() const { return visual_target_temperature_step_; }
  float get_visual_current_temperature_step() const { return visual_current_temperature_step_; }
  void set_visual_target_temperature_step(float temperature_step) {
    visual_target_temperature_step_ = temperature_step;
  }
  void set_visual_current_temperature_step(float temperature_step) {
    visual_current_temperature_step_ = temperature_step;
  }
  void set_visual_temperature_step(float temperature_step) {
    visual_target_temperature_step_ = temperature_step;
    visual_current_temperature_step_ = temperature_step;
  }
  int8_t get_target_temperature_accuracy_decimals() const;
  int8_t get_current_temperature_accuracy_decimals() const;


 protected:

  bool supports_current_temperature_{false};
  bool supports_current_humidity_{false};
  bool supports_two_point_target_temperature_{false};
  bool supports_target_humidity_{false};
  bool supports_action_{false};
  std::set<PelletEcoMode> supported_eco_mode_;
  std::set<PelletHeatLevel> supported_heat_level_;
  float visual_min_temperature_{10};
  float visual_max_temperature_{30};
  float visual_target_temperature_step_{0.1};
  float visual_current_temperature_step_{0.1};
};

}  // namespace climate
}  // namespace esphome
