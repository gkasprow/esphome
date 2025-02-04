#pragma once

#include "esphome/components/switch/switch.h"
#include "../deep_sleep_component.h"

namespace esphome {
namespace deep_sleep {

class GuardSwitch : public switch_::Switch, public Parented<DeepSleepComponent> {
 protected:
  void write_state(bool value) override {
    this->publish_state(value);
    this->parent_->set_guard(value);
  }
};

}  // namespace deep_sleep
}  // namespace esphome
