#pragma once

#include "esphome/core/component.h"
#include "remote_base.h"

#include <vector>

namespace esphome {
namespace remote_base {

struct DaikinData {
  std::vector<uint8_t> data;

  bool operator==(const DaikinData &rhs) const { return data == rhs.data; }
};

class DaikinProtocol : public RemoteProtocol<DaikinData> {
 public:
  void encode(RemoteTransmitData *dst, const DaikinData &data) override;
  optional<DaikinData> decode(RemoteReceiveData src) override;
  void dump(const DaikinData &data) override;

 protected:
  static void encode_byte(RemoteTransmitData *dst, uint8_t byte);
};

DECLARE_REMOTE_PROTOCOL(Daikin);

template<typename... Ts> class DaikinAction : public RemoteTransmitterActionBase<Ts...> {
 public:
  TEMPLATABLE_VALUE(std::vector<uint8_t>, data);

  void set_data(const std::vector<uint8_t> &data) { data_ = data; }
  void encode(RemoteTransmitData *dst, Ts... x) override {
    DaikinData data{};
    data.data = this->data_.value(x...);
    DaikinProtocol().encode(dst, data);
  }
};

}  // namespace remote_base
}  // namespace esphome
