#include "daikin_protocol.h"
#include "esphome/core/log.h"

namespace esphome {
namespace remote_base {

static const char *const TAG = "remote.daikin";

// IR Transmission
static const uint32_t IR_FREQUENCY = 38000;
static const uint32_t HEADER_MARK = 3360;
static const uint32_t HEADER_SPACE = 1760;
static const uint32_t BIT_MARK = 520;
static const uint32_t ONE_SPACE = 1370;
static const uint32_t ZERO_SPACE = 360;
static const uint32_t MESSAGE_SPACE = 32300;

static const uint8_t HEADER[] = {0x11, 0xda, 0x27, 0x00};

void DaikinProtocol::encode(RemoteTransmitData *dst, const DaikinData &data) {
  dst->set_carrier_frequency(IR_FREQUENCY);

  dst->mark(HEADER_MARK);
  dst->space(HEADER_SPACE);

  uint8_t checksum = 0;

  for (size_t i = 0; i < sizeof(HEADER); i++) {
    DaikinProtocol::encode_byte(dst, HEADER[i]);
    checksum += HEADER[i];
  }

  for (uint8_t byte : data.data) {
    DaikinProtocol::encode_byte(dst, byte);
    checksum += byte;
  }

  DaikinProtocol::encode_byte(dst, checksum);

  dst->mark(BIT_MARK);
  dst->space(MESSAGE_SPACE);
}

optional<DaikinData> DaikinProtocol::decode(RemoteReceiveData src) {
  if (!src.expect_item(HEADER_MARK, HEADER_SPACE)) {
    return {};
  }

  DaikinData out;
  bool end_message = false;

  while (src.is_valid() && !end_message) {
    // Fetch each byte
    uint8_t byte = 0;
    for (int8_t bit = 0; bit < 8; bit++) {
      if (src.expect_item(BIT_MARK, ONE_SPACE)) {
        byte |= 1 << bit;
      } else if (!src.expect_item(BIT_MARK, ZERO_SPACE)) {
        end_message = true;
        break;
      }
    }

    if (!end_message)
      out.data.push_back(byte);
  }

  if (out.data.size() <= 4)
    return {};

  // Calculate checksum and check header
  uint8_t checksum = 0, i = 0;
  for (auto it = out.data.begin(); it != std::prev(out.data.end()); i++) {
    checksum += *it;

    if (i < sizeof(HEADER)) {
      if (*it != HEADER[i]) {
        ESP_LOGD(TAG, "Invalid header byte (%hhu): received %02X, expected %02X", i, *it, HEADER[i]);
        return {};
      }

      it = out.data.erase(it);
    } else {
      ++it;
    }
  }

  if (checksum != out.data.back()) {
    ESP_LOGD(TAG, "Invalid checksum: received %02X, expected %02X", out.data.back(), checksum);
    return {};
  }

  out.data.pop_back();
  return out;
}

void DaikinProtocol::dump(const DaikinData &data) {
  ESP_LOGI(TAG, "Received Daikin: %s", format_hex_pretty(data.data).c_str());
}

void DaikinProtocol::encode_byte(RemoteTransmitData *dst, uint8_t byte) {
  for (uint8_t mask = 1; mask > 0; mask <<= 1) {
    dst->mark(BIT_MARK);
    const bool bit = byte & mask;
    dst->space(bit ? ONE_SPACE : ZERO_SPACE);
  }
}

}  // namespace remote_base
}  // namespace esphome
