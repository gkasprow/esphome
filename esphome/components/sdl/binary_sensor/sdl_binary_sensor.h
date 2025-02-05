#pragma once

#ifdef USE_HOST
#include "../sdl_esphome.h"
#include "esphome/components/binary_sensor/binary_sensor.h"

namespace esphome {
namespace sdl {

class SdlBinarySensor : public binary_sensor::BinarySensor, public Component, public Parented<Sdl> {
 public:
  void loop() override { this->publish_state(this->parent_->key_state[this->key_]); }

  void set_key(int key) { this->key_ = key; }

 protected:
  int key_ = 0;
};

}  // namespace sdl
}  // namespace esphome
#endif
