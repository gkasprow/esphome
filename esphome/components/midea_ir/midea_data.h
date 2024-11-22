#pragma once

// TODO: make base, why? Because this file can be used by midea component also. See
// esphome/components/midea/ir_transmitter.h

#include "esphome/components/remote_base/midea_protocol.h"
#include "esphome/components/climate/climate_mode.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace midea_ir {

using climate::ClimateMode;
using climate::ClimateFanMode;
using remote_base::MideaData;

class ControlData : public MideaData {
 public:
  // Default constructor (power: ON, mode: AUTO, fan: AUTO, temp: 25C)
  ControlData() : MideaData({MIDEA_TYPE_CONTROL, 0x82, 0x48, 0xFF, 0xFF}) {}
  // Copy from Base
  ControlData(const MideaData &data) : MideaData(data) {}

  void set_temp(float temp);
  float get_temp() const;

  void set_mode(ClimateMode mode);
  ClimateMode get_mode() const;

  void set_fan_mode(ClimateFanMode mode);
  ClimateFanMode get_fan_mode() const;

  void set_sleep_preset(bool value) { this->set_mask_(1, value, 64); }
  bool get_sleep_preset() const { return this->get_value_(1, 64); }

  void set_fahrenheit(bool value) { this->set_mask_(2, value, 32); }
  bool get_fahrenheit() const { return this->get_value_(2, 32); }

  void fix();

 protected:
  enum Mode : uint8_t {
    MODE_COOL,
    MODE_DRY,
    MODE_AUTO,
    MODE_HEAT,
    MODE_FAN_ONLY,
  };
  enum FanMode : uint8_t {
    FAN_AUTO,
    FAN_LOW,
    FAN_MEDIUM,
    FAN_HIGH,
  };
  void set_fan_mode_(FanMode mode) { this->set_value_(1, mode, 3, 3); }
  FanMode get_fan_mode_() const { return static_cast<FanMode>(this->get_value_(1, 3, 3)); }
  void set_mode_(Mode mode) { this->set_value_(1, mode, 7); }
  Mode get_mode_() const { return static_cast<Mode>(this->get_value_(1, 7)); }
  void set_power_(bool value) { this->set_mask_(1, value, 128); }
  bool get_power_() const { return this->get_value_(1, 128); }
};

class FollowMeData : public MideaData {
 public:
  // Default constructor (temp: 30C, beeper: off)
  FollowMeData() : MideaData({MIDEA_TYPE_FOLLOW_ME, 0x82, 0x48, 0x7F, 0x1F}) {}
  // Copy from Base
  FollowMeData(const MideaData &data) : MideaData(data) {}
  // Direct from temperature in celsius and beeper values
  FollowMeData(uint8_t temp, bool beeper = false) : FollowMeData() {
    this->set_temp(temp, false);
    this->set_beeper(beeper);
  }
  // Direct from temperature, fahrenheit and beeper values
  FollowMeData(uint8_t temp, bool fahrenheit, bool beeper) : FollowMeData() {
    this->set_temp(temp, fahrenheit);
    this->set_beeper(beeper);
  }

  /* TEMPERATURE */
  uint8_t temp() const {
    if (this->fahrenheit()) {
      return this->get_value_(4) + 31;
    }
    return this->get_value_(4) - 1;
  }
  void set_temp(uint8_t val, bool fahrenheit = false) {
    this->set_fahrenheit(fahrenheit);
    if (this->fahrenheit()) {
      // see https://github.com/esphome/feature-requests/issues/1627#issuecomment-1365639966
      val = esphome::clamp<uint8_t>(val, MIN_TEMP_F, MAX_TEMP_F) - 31;
    } else {
      val = esphome::clamp<uint8_t>(val, MIN_TEMP_C, MAX_TEMP_C) + 1;
    }
    this->set_value_(4, val);
  }

  /* BEEPER */
  bool beeper() const { return this->get_value_(3, 128); }
  void set_beeper(bool val) { this->set_mask_(3, val, 128); }

  /* FAHRENHEIT */
  bool fahrenheit() const { return this->get_value_(2, 32); }
  void set_fahrenheit(bool val) { this->set_mask_(2, val, 32); }

 protected:
  static const uint8_t MIN_TEMP_C = 0;
  static const uint8_t MAX_TEMP_C = 37;

  // see
  // https://github.com/crankyoldgit/IRremoteESP8266/blob/9bdf8abcb465268c5409db99dc83a26df64c7445/src/ir_Midea.h#L116
  static const uint8_t MIN_TEMP_F = 32;
  // see
  // https://github.com/crankyoldgit/IRremoteESP8266/blob/9bdf8abcb465268c5409db99dc83a26df64c7445/src/ir_Midea.h#L117
  static const uint8_t MAX_TEMP_F = 99;
};

class SpecialData : public MideaData {
 public:
  SpecialData(uint8_t code) : MideaData({MIDEA_TYPE_SPECIAL, code, 0xFF, 0xFF, 0xFF}) {}
  static const uint8_t VSWING_STEP = 1;
  static const uint8_t VSWING_TOGGLE = 2;
  static const uint8_t TURBO_TOGGLE = 9;
};

}  // namespace midea_ir
}  // namespace esphome
