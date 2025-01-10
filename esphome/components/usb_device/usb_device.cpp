#if defined(USE_ESP32_VARIANT_ESP32S2) || defined(USE_ESP32_VARIANT_ESP32S3)
#include "usb_device.h"
#include "esphome/core/log.h"
#include "Adafruit_TinyUSB.h"
#include "USB.h"

namespace esphome {
namespace usb_device {

static const char *const TAG = "usb_device";

void UsbDevice::setup() {
#ifdef USE_VENDOR_ID
  USB.VID(this->vendor_id_);
#endif
#ifdef USE_PRODUCT_ID
  USB.PID(this->product_id_);
#endif
#ifdef USE_MANUFACTURER_NAME
  USB.manufacturerName(this->manufacturer_name_.c_str());
#endif
#ifdef USE_PRODUCT_NAME
  USB.productName(this->product_name_.c_str());
#endif
  USB.begin();
}

void UsbDevice::update() {
#ifdef USE_BINARY_SENSOR
  if (mounted_ != nullptr) {
    mounted_->publish_state(TinyUSBDevice.mounted());
  }
  if (ready_ != nullptr) {
    ready_->publish_state(TinyUSBDevice.ready());
  }
  if (suspended_ != nullptr) {
    suspended_->publish_state(TinyUSBDevice.suspended());
  }
#endif
}

float UsbDevice::get_setup_priority() const {
  // it should be registered after all USB components
  return setup_priority::HARDWARE - 100;
}

void UsbDevice::dump_config() {
  ESP_LOGCONFIG(TAG, "USB device - mounted: %s, suspended: %s, ready: %s", YESNO(TinyUSBDevice.mounted()),
                YESNO(TinyUSBDevice.suspended()), YESNO(TinyUSBDevice.ready()));
}

#ifdef USE_VENDOR_ID
void UsbDevice::set_vendor_id(const uint16_t vid) { this->vendor_id_ = vid; }
#endif
#ifdef USE_PRODUCT_ID
void UsbDevice::set_product_id(const uint16_t pid) { this->product_id_ = pid; }
#endif
#ifdef USE_MANUFACTURER_NAME
void UsbDevice::set_manufacturer_name(const std::string &manufacturer_name) { this->manufacturer_name_ = manufacturer_name; }
#endif
#ifdef USE_PRODUCT_NAME
void UsbDevice::set_product_name(const std::string &product_name) { this->product_name_ = product_name; }
#endif

#ifdef USE_BINARY_SENSOR
void UsbDevice::set_mounted_binary_sensor(binary_sensor::BinarySensor *sensor) { mounted_ = sensor; };
void UsbDevice::set_ready_binary_sensor(binary_sensor::BinarySensor *sensor) { ready_ = sensor; };
void UsbDevice::set_suspended_binary_sensor(binary_sensor::BinarySensor *sensor) { suspended_ = sensor; };
#endif

}  // namespace usb_device
}  // namespace esphome
#endif
