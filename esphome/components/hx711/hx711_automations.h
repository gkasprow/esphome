#pragma once

#include "esphome/core/automation.h"
#include "hx711.h"

namespace esphome {
namespace hx711 {

template<typename... Ts> class HX711SensorActionBase : public Action<Ts...> {
 public:
  void set_parent(HX711Sensor *parent) { this->parent_ = parent; }

 protected:
  HX711Sensor *parent_;
};

template<typename... Ts> class PowerUpAction : public HX711SensorActionBase<Ts...> {
 public:
  void play(Ts... x) override { this->parent_->power_up(); }
};

template<typename... Ts> class PowerDownAction : public HX711SensorActionBase<Ts...> {
 public:
  void play(Ts... x) override { this->parent_->power_down(); }
};

}  // namespace hx711
}  // namespace esphome