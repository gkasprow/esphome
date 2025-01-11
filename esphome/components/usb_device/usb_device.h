#pragma once
#if defined(USE_ESP32_VARIANT_ESP32S2) || defined(USE_ESP32_VARIANT_ESP32S3)
#include "esphome/core/component.h"
#include "esphome/core/defines.h"
#ifdef USE_BINARY_SENSOR
#include "esphome/components/binary_sensor/binary_sensor.h"
#endif
namespace esphome {
namespace usb_device {

class UsbDevice : public PollingComponent {
 public:
  void setup() override;
  void update() override;
  float get_setup_priority() const override;
  void dump_config() override;
  void set_vendor_id(uint16_t vendor_id);
  void set_product_id(uint16_t product_id);
  void set_manufacturer_name(const std::string &manufacturer_name);
  void set_product_name(const std::string &product_name);
#ifdef USE_BINARY_SENSOR
  void set_mounted_binary_sensor(binary_sensor::BinarySensor *sensor);
  void set_ready_binary_sensor(binary_sensor::BinarySensor *sensor);
  void set_suspended_binary_sensor(binary_sensor::BinarySensor *sensor);
#endif
 protected:
  uint16_t vendor_id_;
  uint16_t product_id_;
  std::string manufacturer_name_;
  std::string product_name_;
#ifdef USE_BINARY_SENSOR
  binary_sensor::BinarySensor *mounted_;
  binary_sensor::BinarySensor *ready_;
  binary_sensor::BinarySensor *suspended_;
#endif
};

}  // namespace usb_device
}  // namespace esphome
#endif
