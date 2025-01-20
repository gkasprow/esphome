#include "tuya_pellet.h"
#include "esphome/core/log.h"

namespace esphome {
namespace tuya {

static const char *const TAG = "tuya.pellet";

void TuyaPellet::setup() {
  if (this->switch_id_.has_value()) {
    this->parent_->register_listener(*this->switch_id_, [this](const TuyaDatapoint &datapoint) {
      ESP_LOGV(TAG, "MCU reported switch is: %s", ONOFF(datapoint.value_bool));
      this->mode = climate::CLIMATE_MODE_OFF;
      if (datapoint.value_bool) {
        if (this->supports_heat_ && this->supports_cool_) {
          this->mode = climate::CLIMATE_MODE_HEAT_COOL;
        } else if (this->supports_heat_) {
          this->mode = climate::CLIMATE_MODE_HEAT;
        } else if (this->supports_cool_) {
          this->mode = climate::CLIMATE_MODE_COOL;
        }
      }
      this->compute_state_();
      this->publish_state();
    });
  }
  if (this->active_state_id_.has_value()) {
    this->parent_->register_listener(*this->active_state_id_, [this](const TuyaDatapoint &datapoint) {
      ESP_LOGV(TAG, "MCU reported active state is: %u", datapoint.value_enum);
      this->active_state_ = datapoint.value_enum;
      this->compute_state_();
      this->publish_state();
    });
  }
  if (this->target_temperature_id_.has_value()) {
    this->parent_->register_listener(*this->target_temperature_id_, [this](const TuyaDatapoint &datapoint) {
      this->manual_temperature_ = datapoint.value_int * this->target_temperature_multiplier_;
      if (this->reports_fahrenheit_) {
        this->manual_temperature_ = (this->manual_temperature_ - 32) * 5 / 9;
      }

      ESP_LOGV(TAG, "MCU reported manual target temperature is: %.1f", this->manual_temperature_);
      this->compute_target_temperature_();
      this->compute_state_();
      this->publish_state();
    });
  }
  if (this->current_temperature_id_.has_value()) {
    this->parent_->register_listener(*this->current_temperature_id_, [this](const TuyaDatapoint &datapoint) {
      this->current_temperature = datapoint.value_int * this->current_temperature_multiplier_;
      if (this->reports_fahrenheit_) {
        this->current_temperature = (this->current_temperature - 32) * 5 / 9;
      }

      ESP_LOGV(TAG, "MCU reported current temperature is: %.1f", this->current_temperature);
      this->compute_state_();
      this->publish_state();
    });
  }

  if (this->eco_mode_id_.has_value()) {
    this->parent_->register_listener(*this->eco_mode_id_, [this](const TuyaDatapoint &datapoint) {
      ESP_LOGV(TAG, "MCU reported Eco Mode is: %u", datapoint.value_enum);
      this->eco_mode_state_ = datapoint.value_enum;
      this->compute_ecomode_();
      this->publish_state();
    });
  }

  if (this->heat_level_id_.has_value()) {
    this->parent_->register_listener(*this->heat_level_id_, [this](const TuyaDatapoint &datapoint) {
      ESP_LOGV(TAG, "MCU reported Heat Level is: %u", datapoint.value_enum);
      this->heat_level_state_ = datapoint.value_enum;
      this->compute_heatlevel_();
      this->publish_state();
    });
  }
}

void TuyaPellet::loop() {
  bool state_changed = false;
  if (state_changed) {
    this->compute_state_();
    this->publish_state();
  }
}

void TuyaPellet::control(const climate::ClimateCall &call) {
  if (call.get_mode().has_value()) {
    const bool switch_state = *call.get_mode() != climate::CLIMATE_MODE_OFF;
    ESP_LOGV(TAG, "Setting switch: %s", ONOFF(switch_state));
    this->parent_->set_boolean_datapoint_value(*this->switch_id_, switch_state);
    const climate::ClimateMode new_mode = *call.get_mode();

    if (this->active_state_id_.has_value()) {
      if (new_mode == climate::CLIMATE_MODE_HEAT && this->supports_heat_) {
        this->parent_->set_enum_datapoint_value(*this->active_state_id_, *this->active_state_heating_value_);
      } else if (new_mode == climate::CLIMATE_MODE_COOL && this->supports_cool_) {
        this->parent_->set_enum_datapoint_value(*this->active_state_id_, *this->active_state_cooling_value_);
      } else if (new_mode == climate::CLIMATE_MODE_DRY && this->active_state_drying_value_.has_value()) {
        this->parent_->set_enum_datapoint_value(*this->active_state_id_, *this->active_state_drying_value_);
      } else if (new_mode == climate::CLIMATE_MODE_FAN_ONLY && this->active_state_fanonly_value_.has_value()) {
        this->parent_->set_enum_datapoint_value(*this->active_state_id_, *this->active_state_fanonly_value_);
      }
    } else {
      ESP_LOGW(TAG, "Active state (mode) datapoint not configured");
    }
  }

  control_eco_mode_(call);
  control_heat_level_(call);

  if (call.get_target_temperature().has_value()) {
    float target_temperature = *call.get_target_temperature();
    if (this->reports_fahrenheit_)
      target_temperature = (target_temperature * 9 / 5) + 32;

    ESP_LOGV(TAG, "Setting target temperature: %.1f", target_temperature);
    this->parent_->set_integer_datapoint_value(*this->target_temperature_id_,
                                               (int) (target_temperature / this->target_temperature_multiplier_));
  }
}

void TuyaPellet::control_eco_mode_(const climate::ClimateCall &call) {
  if (call.get_eco_mode().has_value()) {
    climate::ClimateFanMode fan_mode = *call.get_fan_mode();

    uint8_t tuya_fan_speed;
    switch (fan_mode) {
      case climate::CLIMATE_FAN_LOW:
        tuya_fan_speed = *fan_speed_low_value_;
        break;
      case climate::CLIMATE_FAN_MEDIUM:
        tuya_fan_speed = *fan_speed_medium_value_;
        break;
      case climate::CLIMATE_FAN_MIDDLE:
        tuya_fan_speed = *fan_speed_middle_value_;
        break;
      case climate::CLIMATE_FAN_HIGH:
        tuya_fan_speed = *fan_speed_high_value_;
        break;
      case climate::CLIMATE_FAN_AUTO:
        tuya_fan_speed = *fan_speed_auto_value_;
        break;
      default:
        tuya_fan_speed = 0;
        break;
    }

    if (this->fan_speed_id_.has_value()) {
      this->parent_->set_enum_datapoint_value(*this->fan_speed_id_, tuya_fan_speed);
    }
  }
}

void TuyaPellet::control_heat_level_(const climate::ClimateCall &call) {
  if (call.get_fan_mode().has_value()) {
    climate::ClimateFanMode fan_mode = *call.get_fan_mode();

    uint8_t tuya_fan_speed;
    switch (fan_mode) {
      case climate::CLIMATE_FAN_LOW:
        tuya_fan_speed = *fan_speed_low_value_;
        break;
      case climate::CLIMATE_FAN_MEDIUM:
        tuya_fan_speed = *fan_speed_medium_value_;
        break;
      case climate::CLIMATE_FAN_MIDDLE:
        tuya_fan_speed = *fan_speed_middle_value_;
        break;
      case climate::CLIMATE_FAN_HIGH:
        tuya_fan_speed = *fan_speed_high_value_;
        break;
      case climate::CLIMATE_FAN_AUTO:
        tuya_fan_speed = *fan_speed_auto_value_;
        break;
      default:
        tuya_fan_speed = 0;
        break;
    }

    if (this->fan_speed_id_.has_value()) {
      this->parent_->set_enum_datapoint_value(*this->fan_speed_id_, tuya_fan_speed);
    }
  }
}

climate::ClimateTraits TuyaPellet::traits() {
  auto traits = climate::ClimateTraits();
  traits.set_supports_action(true);
  traits.set_supports_current_temperature(this->current_temperature_id_.has_value());
  if (supports_heat_)
    traits.add_supported_mode(climate::CLIMATE_MODE_HEAT);
  if (supports_cool_)
    traits.add_supported_mode(climate::CLIMATE_MODE_COOL);
  return traits;
  }

tuya_pellet::PelletTraits TuyaPellet::p_traits() {
  auto p_traits = tuya_pellet::PelletTraits();
  if (eco_mode_id_) {
    if (e1_value_)
      pellet_traits.add_supported_eco_mode(CLIMATE_ECO1);
    if (e2_value_)
      pellet_traits.add_supported_eco_mode(CLIMATE_ECO2);
  }
  if (heat_level_id_) {
    if (p1_value_)
      pellet_traits.add_supported_heat_level(CLIMATE_HEAT1);
    if (p2_value_)
      pellet_traits.add_supported_heat_level(CLIMATE_HEAT2);
    if (p3_value_)
      pellet_traits.add_supported_heat_level(CLIMATE_HEAT3);
    if (p4_value_)
      pellet_traits.add_supported_heat_level(CLIMATE_HEAT4);
  }
  return p_traits;
}

void TuyaPellet::dump_config() {
  LOG_CLIMATE("", "Tuya Climate", this);
  if (this->switch_id_.has_value()) {
    ESP_LOGCONFIG(TAG, "  Switch has datapoint ID %u", *this->switch_id_);
  }
  if (this->active_state_id_.has_value()) {
    ESP_LOGCONFIG(TAG, "  Active state has datapoint ID %u", *this->active_state_id_);
  }
  if (this->target_temperature_id_.has_value()) {
    ESP_LOGCONFIG(TAG, "  Target Temperature has datapoint ID %u", *this->target_temperature_id_);
  }
  if (this->current_temperature_id_.has_value()) {
    ESP_LOGCONFIG(TAG, "  Current Temperature has datapoint ID %u", *this->current_temperature_id_);
  }
}

void TuyaPellet::compute_ecomode_() {
  if (this->eco_mode_id_.has_value()) {
    // Use state from MCU datapoint
    if (this->e1_value_.has_value() && this->eco_mode_state_ == this->e1_value_) {
      this->eco_mode = CLIMATE_ECO1;
    } else if (this->e2_value_.has_value() && this->eco_mode_state_ == this->e2_value_) {
      this->eco_mode = CLIMATE_ECO2;
    }
  }
}

void TuyaPellet::compute_heatlevel_() {
  if (this->heat_level_id_.has_value()) {
    // Use state from MCU datapoint
    if (this->p1_value_.has_value() && this->heat_level_state_ == this->p1_value_) {
      this->heat_level = CLIMATE_HEAT1;
    } else if (this->p2_value_.has_value() && this->heat_level_state_ == this->p2_value_) {
      this->heat_level = CLIMATE_HEAT2;
    } else if (this->p3_value_.has_value() && this->heat_level_state_ == this->p3_value_) {
      this->heat_level = CLIMATE_HEAT3;
    } else if (this->p4_value_.has_value() && this->heat_level_state_ == this->p4_value_) {
      this->heat_level = CLIMATE_HEAT4;
    }
  }
}

void TuyaPellet::compute_state_() {
  if (std::isnan(this->current_temperature) || std::isnan(this->target_temperature)) {
    // if any control parameters are nan, go to OFF action (not IDLE!)
    this->switch_to_action_(climate::CLIMATE_ACTION_OFF);
    return;
  }

  if (this->mode == climate::CLIMATE_MODE_OFF) {
    this->switch_to_action_(climate::CLIMATE_ACTION_OFF);
    return;
  }

  climate::ClimateAction target_action = climate::CLIMATE_ACTION_IDLE;

  if (this->active_state_id_.has_value()) {
    // Use state from MCU datapoint
    if (this->supports_heat_ && this->active_state_heating_value_.has_value() &&
        this->active_state_ == this->active_state_heating_value_) {
      target_action = climate::CLIMATE_ACTION_HEATING;
      this->mode = climate::CLIMATE_MODE_HEAT;
    } else if (this->supports_cool_ && this->active_state_cooling_value_.has_value() &&
               this->active_state_ == this->active_state_cooling_value_) {
      target_action = climate::CLIMATE_ACTION_COOLING;
      this->mode = climate::CLIMATE_MODE_COOL;
    }
  } else {
    // Fallback to active state calc based on temp and hysteresis
    const float temp_diff = this->target_temperature - this->current_temperature;
    if (std::abs(temp_diff) > this->hysteresis_) {
      if (this->supports_heat_ && temp_diff > 0) {
        target_action = climate::CLIMATE_ACTION_HEATING;
        this->mode = climate::CLIMATE_MODE_HEAT;
      } else if (this->supports_cool_ && temp_diff < 0) {
        target_action = climate::CLIMATE_ACTION_COOLING;
        this->mode = climate::CLIMATE_MODE_COOL;
      }
    }
  }

  this->switch_to_action_(target_action);
}

void TuyaPellet::switch_to_action_(climate::ClimateAction action) {
  // For now this just sets the current action but could include triggers later
  this->action = action;
}

}  // namespace tuya
}  // namespace esphome
