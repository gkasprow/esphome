#pragma once

#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>
#include "constants.h"
#include "span.h"

namespace esphome {
namespace sml {

using bytes = std::vector<uint8_t>;
using byte_span = span<uint8_t>;

class SmlNode {
 public:
  SmlNode(uint8_t type_, byte_span bytes_) : type(type_), value_bytes(bytes_) {}
  SmlNode(uint8_t type_, std::vector<SmlNode> &&nodes_) : type(type_), nodes(nodes_) {}

  const uint8_t type;
  const byte_span value_bytes;
  const std::vector<SmlNode> nodes;
};

class ObisInfo {
 public:
  ObisInfo(byte_span const &server_id, SmlNode const &val_list_entry);
  byte_span server_id;
  byte_span code;
  byte_span status;
  char unit;
  char scaler;
  byte_span value;
  uint16_t value_type;
  std::string code_repr() const;
};

class SmlFile {
 public:
  SmlFile(byte_span const &buffer);
  bool setup_node(std::vector<SmlNode> &nodes);
  std::vector<SmlNode> messages;
  std::vector<ObisInfo> get_obis_info();

 protected:
  const byte_span buffer_;
  size_t pos_;
};

std::string bytes_repr(const byte_span &buffer);

uint64_t bytes_to_uint(const byte_span &buffer);

int64_t bytes_to_int(const byte_span &buffer);

std::string bytes_to_string(const byte_span &buffer);
}  // namespace sml
}  // namespace esphome
