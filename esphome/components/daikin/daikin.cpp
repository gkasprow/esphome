#include "daikin.h"
#include "esphome/components/remote_base/daikin_protocol.h"

namespace esphome {
namespace daikin {

static const char *const TAG = "daikin.climate";

void DaikinClimate::transmit_state() {
  const remote_base::DaikinData frame_1{.data = {0xC5, 0x00, 0x00}};
  const remote_base::DaikinData frame_2{.data = {0x42, 0x49, 0x05}};

  remote_base::DaikinData frame_3{
      .data = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x60, 0x00, 0x00, 0xC0, 0x00, 0x00}};

  frame_3.data[1] = this->operation_mode_();
  frame_3.data[2] = this->temperature_();
  uint16_t fan_speed = this->fan_speed_();
  frame_3.data[4] = fan_speed >> 8;
  frame_3.data[5] = fan_speed & 0xff;

  this->transmit_<remote_base::DaikinProtocol>(frame_1);
  this->transmit_<remote_base::DaikinProtocol>(frame_2);
  this->transmit_<remote_base::DaikinProtocol>(frame_3);
}

uint8_t DaikinClimate::operation_mode_() {
  uint8_t operating_mode = DAIKIN_MODE_ON;
  switch (this->mode) {
    case climate::CLIMATE_MODE_COOL:
      operating_mode |= DAIKIN_MODE_COOL;
      break;
    case climate::CLIMATE_MODE_DRY:
      operating_mode |= DAIKIN_MODE_DRY;
      break;
    case climate::CLIMATE_MODE_HEAT:
      operating_mode |= DAIKIN_MODE_HEAT;
      break;
    case climate::CLIMATE_MODE_HEAT_COOL:
      operating_mode |= DAIKIN_MODE_AUTO;
      break;
    case climate::CLIMATE_MODE_FAN_ONLY:
      operating_mode |= DAIKIN_MODE_FAN;
      break;
    case climate::CLIMATE_MODE_OFF:
    default:
      operating_mode = DAIKIN_MODE_OFF;
      break;
  }

  return operating_mode;
}

uint16_t DaikinClimate::fan_speed_() {
  uint16_t fan_speed;
  switch (this->fan_mode.value()) {
    case climate::CLIMATE_FAN_LOW:
      fan_speed = DAIKIN_FAN_1 << 8;
      break;
    case climate::CLIMATE_FAN_MEDIUM:
      fan_speed = DAIKIN_FAN_3 << 8;
      break;
    case climate::CLIMATE_FAN_HIGH:
      fan_speed = DAIKIN_FAN_5 << 8;
      break;
    case climate::CLIMATE_FAN_AUTO:
    default:
      fan_speed = DAIKIN_FAN_AUTO << 8;
  }

  // If swing is enabled switch first 4 bits to 1111
  switch (this->swing_mode) {
    case climate::CLIMATE_SWING_VERTICAL:
      fan_speed |= 0x0F00;
      break;
    case climate::CLIMATE_SWING_HORIZONTAL:
      fan_speed |= 0x000F;
      break;
    case climate::CLIMATE_SWING_BOTH:
      fan_speed |= 0x0F0F;
      break;
    default:
      break;
  }
  return fan_speed;
}

uint8_t DaikinClimate::temperature_() {
  // Force special temperatures depending on the mode
  switch (this->mode) {
    case climate::CLIMATE_MODE_FAN_ONLY:
      return 0x32;
    case climate::CLIMATE_MODE_HEAT_COOL:
    case climate::CLIMATE_MODE_DRY:
      return 0xc0;
    default:
      uint8_t temperature = (uint8_t) roundf(clamp<float>(this->target_temperature, DAIKIN_TEMP_MIN, DAIKIN_TEMP_MAX));
      return temperature << 1;
  }
}

bool DaikinClimate::on_receive(remote_base::RemoteReceiveData data) {
  auto decoded = remote_base::DaikinProtocol().decode(data);
  if (!decoded.has_value())
    return false;

  const remote_base::DaikinData &state = decoded.value();
  if (state.data.size() != 14 || state.data[0] != 0)  // Frame type
    return false;

  uint8_t mode = state.data[1];
  if (mode & DAIKIN_MODE_ON) {
    switch (mode & 0xF0) {
      case DAIKIN_MODE_COOL:
        this->mode = climate::CLIMATE_MODE_COOL;
        break;
      case DAIKIN_MODE_DRY:
        this->mode = climate::CLIMATE_MODE_DRY;
        break;
      case DAIKIN_MODE_HEAT:
        this->mode = climate::CLIMATE_MODE_HEAT;
        break;
      case DAIKIN_MODE_AUTO:
        this->mode = climate::CLIMATE_MODE_HEAT_COOL;
        break;
      case DAIKIN_MODE_FAN:
        this->mode = climate::CLIMATE_MODE_FAN_ONLY;
        break;
    }
  } else {
    this->mode = climate::CLIMATE_MODE_OFF;
  }

  uint8_t temperature = state.data[2];
  if (!(temperature & 0xC0)) {
    this->target_temperature = temperature >> 1;
  }

  uint8_t fan_mode = state.data[4];
  uint8_t swing_mode = state.data[5];
  if (fan_mode & 0xF && swing_mode & 0xF) {
    this->swing_mode = climate::CLIMATE_SWING_BOTH;
  } else if (fan_mode & 0xF) {
    this->swing_mode = climate::CLIMATE_SWING_VERTICAL;
  } else if (swing_mode & 0xF) {
    this->swing_mode = climate::CLIMATE_SWING_HORIZONTAL;
  } else {
    this->swing_mode = climate::CLIMATE_SWING_OFF;
  }
  switch (fan_mode & 0xF0) {
    case DAIKIN_FAN_1:
    case DAIKIN_FAN_2:
    case DAIKIN_FAN_SILENT:
      this->fan_mode = climate::CLIMATE_FAN_LOW;
      break;
    case DAIKIN_FAN_3:
      this->fan_mode = climate::CLIMATE_FAN_MEDIUM;
      break;
    case DAIKIN_FAN_4:
    case DAIKIN_FAN_5:
      this->fan_mode = climate::CLIMATE_FAN_HIGH;
      break;
    case DAIKIN_FAN_AUTO:
      this->fan_mode = climate::CLIMATE_FAN_AUTO;
      break;
  }
  this->publish_state();
  return true;
}

}  // namespace daikin
}  // namespace esphome
