#include "nec_protocol.h"
#include "esphome/core/log.h"

namespace esphome {
namespace remote_base {

static const char *const TAG = "remote.nec";

// NEC IR Protocol
// Protocol Reference: https://techdocs.altium.com/display/FPGA/NEC%2bInfrared%2bTransmission%2bProtocol

// Timing constants in microseconds
static const uint32_t HEADER_HIGH_US = 9000;        // AGC burst: 9ms HIGH
static const uint32_t HEADER_LOW_US = 4500;         // AGC burst: 4.5ms LOW
static const uint32_t BIT_HIGH_US = 562;            // Bit HIGH duration: 562.5µs
static const uint32_t BIT_ONE_LOW_US = 1687;        // Logic '1': 562.5µs HIGH + 1687.5µs LOW
static const uint32_t BIT_ZERO_LOW_US = 562;        // Logic '0': 562.5µs HIGH + 562.5µs LOW
static const uint32_t SPACE_INTER_FRAME_US = 40000;  // Inter-frame space: 40ms
static const uint32_t SPACE_AGC_REPEAT_US = 96187; // AGC Repeat space: ~96.1875ms

// TODO: command_repeats -> repeats (repeat as repeat code, not whole message frame)
void NECProtocol::encode(RemoteTransmitData *dst, const NECData &data) {
  ESP_LOGD(TAG, "Sending NEC: address=0x%04X, command=0x%04X, command_repeats=%d", 
           data.address, data.command, data.command_repeats);

  // Reserve: AGC Header (2) + Address bits (32) + Command bits (32) + Final mark (2) + Repeat codes (4 per repeat)
  dst->reserve(2 + 32 + 32 + 2 + data.command_repeats * 4);
  dst->set_carrier_frequency(38222);

  // Send the AGC Header (start of frame)
  dst->item(HEADER_HIGH_US, HEADER_LOW_US);

  // Encode 16-bit Address
  for (uint16_t mask = 1; mask; mask <<= 1) {
    if (data.address & mask) {
      dst->item(BIT_HIGH_US, BIT_ONE_LOW_US);  // Logic '1'
    } else {
      dst->item(BIT_HIGH_US, BIT_ZERO_LOW_US); // Logic '0'
    }
  }

  // Encode 16-bit Command
  for (uint16_t mask = 1; mask; mask <<= 1) {
    if (data.command & mask) {
      dst->item(BIT_HIGH_US, BIT_ONE_LOW_US);  // Logic '1'
    } else {
      dst->item(BIT_HIGH_US, BIT_ZERO_LOW_US); // Logic '0'
    }
  }

  // Final mark to end the message frame
  dst->mark(BIT_HIGH_US);

  // Space between message frame and first AGC repeat code
  if (data.command_repeats > 0) {
    dst->space(SPACE_INTER_FRAME_US);
  }

  // Send AGC Repeat Codes if requested
  for (uint16_t repeats = 0; repeats < data.command_repeats; repeats++) {
    // AGC Repeat header (shorter version of the initial AGC header)
    dst->item(HEADER_HIGH_US, HEADER_LOW_US / 2);  // Shortened AGC header
    dst->mark(BIT_HIGH_US);  // Mark to complete the repeat code

    // Add space after repeat code, except after the final repeat
    if (repeats < data.command_repeats - 1) {
      dst->space(SPACE_AGC_REPEAT_US);
    }
  }
}

// TODO: command_repeats -> repeats (repeat as repeat code, not whole message frame)
optional<NECData> NECProtocol::decode(RemoteReceiveData src) {
  NECData data{
      .address = 0,
      .command = 0,
      .command_repeats = 0, // Start with 0, as the first frame is counted explicitly
  };

  // Validate the AGC header (start of frame)
  if (!src.expect_item(HEADER_HIGH_US, HEADER_LOW_US)) {
    return {};
  }

  // Decode 16-bit Address
  for (uint16_t mask = 1; mask; mask <<= 1) {
    if (src.expect_item(BIT_HIGH_US, BIT_ONE_LOW_US)) {
      data.address |= mask;  // Logic '1'
    } else if (src.expect_item(BIT_HIGH_US, BIT_ZERO_LOW_US)) {
      data.address &= ~mask; // Logic '0'
    } else {
      return {};
    }
  }

  // Decode 16-bit Command
  for (uint16_t mask = 1; mask; mask <<= 1) {
    if (src.expect_item(BIT_HIGH_US, BIT_ONE_LOW_US)) {
      data.command |= mask;  // Logic '1'
    } else if (src.expect_item(BIT_HIGH_US, BIT_ZERO_LOW_US)) {
      data.command &= ~mask; // Logic '0'
    } else {
      return {};
    }
  }

  // Validate the final mark (end of the message frame)
  if (!src.expect_mark(BIT_HIGH_US)) {
    return {};
  }

  // Check for inter-frame space (only if repeats are expected)
  if (src.peek_space(SPACE_INTER_FRAME_US)) {
    src.expect_space(SPACE_INTER_FRAME_US); // Consume inter-frame space
  }

  // Decode AGC Repeat Codes
  while (src.peek_item(HEADER_HIGH_US, HEADER_LOW_US / 2)) {
    // Validate the repeat header
    if (!src.expect_item(HEADER_HIGH_US, HEADER_LOW_US / 2)) {
      break;
    }

    // Validate the repeat mark
    if (!src.expect_mark(BIT_HIGH_US)) {
      break;
    }

    // Increment the repeat count
    data.command_repeats += 1;

    // Check for space between repeat codes
    if (src.peek_space(SPACE_AGC_REPEAT_US)) {
      src.expect_space(SPACE_AGC_REPEAT_US); // Consume space between repeats
    }
  }

  return data;
}

void NECProtocol::dump(const NECData &data) {
  ESP_LOGI(TAG, "Received NEC: address=0x%04X, command=0x%04X command_repeats=%d", data.address, data.command,
           data.command_repeats);
}

}  // namespace remote_base
}  // namespace esphome
