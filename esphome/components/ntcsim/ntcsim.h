#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#ifdef USE_OUTPUT
#include "esphome/components/output/float_output.h"
#endif

namespace esphome {
namespace ntcsim {

class NTCSimulation : public Component, public sensor::Sensor {
 public:
  void set_sensor(Sensor *sensor) { sensor_ = sensor; }
#ifdef USE_OUTPUT
  void set_output(output::FloatOutput *output) { output_ = output; }
  void set_output_resistance(float resistance) { resistance_ = resistance; }
#endif
  void set_a(double a) { a_ = a; }
  void set_b(double b) { b_ = b; }
  void set_c(double c) { c_ = c; }
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;

 protected:
  void process_(float value);

  sensor::Sensor *sensor_;
  double a_;
  double b_;
  double c_;
#ifdef USE_OUTPUT
  output::FloatOutput *output_{nullptr};
  float resistance_{1.0f};
#endif
};

}  // namespace ntcsim
}  // namespace esphome
