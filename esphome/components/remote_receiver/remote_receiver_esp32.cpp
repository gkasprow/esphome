#include "remote_receiver.h"
#include "esphome/core/log.h"

#ifdef USE_ESP32

namespace esphome {
namespace remote_receiver {

static const char *const TAG = "remote_receiver.esp32";

static bool IRAM_ATTR HOT rmt_callback(rmt_channel_handle_t channel, const rmt_rx_done_event_data_t *data,
                                       void *user_data) {
  RemoteReceiverComponentStore *store = (RemoteReceiverComponentStore *) user_data;
  store->data = *data;
  store->done = true;
  return false;
}

void RemoteReceiverComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Remote Receiver...");
  this->pin_->setup();
  if (this->filter_us_ == 0) {
    this->store_.config.signal_range_min_ns = 0;
  } else {
    this->store_.config.signal_range_min_ns = 500;  // std::min(this->filter_us_, (uint32_t) 255) * 1000;
  }
  this->store_.config.signal_range_max_ns = std::min(this->idle_us_, (uint32_t) 65535) * 1000;

  rmt_rx_channel_config_t rmt{};
  rmt.clk_src = RMT_CLK_SRC_DEFAULT;
  rmt.resolution_hz = 1 * 1000 * 1000;
  rmt.mem_block_symbols = 64 * this->mem_block_num_;
  rmt.gpio_num = gpio_num_t(this->pin_->get_pin());
  esp_err_t error = rmt_new_rx_channel(&rmt, &this->channel_);
  if (error != ESP_OK) {
    this->error_code_ = error;
    this->error_string_ = "in rmt_new_rx_channel";
    this->mark_failed();
    return;
  }
  error = rmt_enable(this->channel_);
  if (error != ESP_OK) {
    this->error_code_ = error;
    this->error_string_ = "in rmt_enable";
    this->mark_failed();
    return;
  }

  rmt_rx_event_callbacks_t callbacks{};
  callbacks.on_recv_done = rmt_callback;
  error = rmt_rx_register_event_callbacks(this->channel_, &callbacks, &this->store_);
  if (error != ESP_OK) {
    this->error_code_ = error;
    this->error_string_ = "in rmt_rx_register_event_callbacks";
    this->mark_failed();
    return;
  }

  this->store_.buffer = new uint32_t[this->buffer_size_ / 4];
  error = rmt_receive(this->channel_, this->store_.buffer, 64 * this->mem_block_num_ * sizeof(rmt_symbol_word_t),
                      &this->store_.config);
  if (error != ESP_OK) {
    this->error_code_ = error;
    this->error_string_ = "in rmt_receive";
    this->mark_failed();
    return;
  }
}

void RemoteReceiverComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "Remote Receiver:");
  LOG_PIN("  Pin: ", this->pin_);
  if (this->pin_->digital_read()) {
    ESP_LOGW(TAG, "Remote Receiver Signal starts with a HIGH value. Usually this means you have to "
                  "invert the signal using 'inverted: True' in the pin schema!");
  }
  ESP_LOGCONFIG(TAG, "  RMT memory blocks: %d", this->mem_block_num_);
  ESP_LOGCONFIG(TAG, "  Clock divider: %u", this->clock_divider_);
  ESP_LOGCONFIG(TAG, "  Tolerance: %" PRIu32 "%s", this->tolerance_,
                (this->tolerance_mode_ == remote_base::TOLERANCE_MODE_TIME) ? " us" : "%");
  ESP_LOGCONFIG(TAG, "  Filter out pulses shorter than: %" PRIu32 " us", this->filter_us_);
  ESP_LOGCONFIG(TAG, "  Signal is done after %" PRIu32 " us of no changes", this->idle_us_);
  if (this->is_failed()) {
    ESP_LOGE(TAG, "Configuring RMT driver failed: %s (%s)", esp_err_to_name(this->error_code_),
             this->error_string_.c_str());
  }
}

void RemoteReceiverComponent::loop() {
  if (this->store_.done) {
    this->decode_rmt_((rmt_symbol_word_t *) this->store_.buffer, this->store_.data.num_symbols);
    this->store_.done = false;
    rmt_receive(this->channel_, this->store_.buffer, sizeof(rmt_symbol_word_t) * 64 * 4, &this->store_.config);
    if (!this->temp_.empty()) {
      this->temp_.push_back(-this->idle_us_);
      this->call_listeners_dumpers_();
    }
  }
}

void RemoteReceiverComponent::decode_rmt_(rmt_symbol_word_t *item, size_t item_count) {
  bool prev_level = false;
  uint32_t prev_length = 0;
  this->temp_.clear();
  int32_t multiplier = this->pin_->is_inverted() ? -1 : 1;
  uint32_t filter_ticks = this->from_microseconds_(this->filter_us_);

  ESP_LOGVV(TAG, "START:");
  for (size_t i = 0; i < item_count; i++) {
    if (item[i].level0) {
      ESP_LOGVV(TAG, "%zu A: ON %" PRIu32 "us (%u ticks)", i, this->to_microseconds_(item[i].duration0),
                item[i].duration0);
    } else {
      ESP_LOGVV(TAG, "%zu A: OFF %" PRIu32 "us (%u ticks)", i, this->to_microseconds_(item[i].duration0),
                item[i].duration0);
    }
    if (item[i].level1) {
      ESP_LOGVV(TAG, "%zu B: ON %" PRIu32 "us (%u ticks)", i, this->to_microseconds_(item[i].duration1),
                item[i].duration1);
    } else {
      ESP_LOGVV(TAG, "%zu B: OFF %" PRIu32 "us (%u ticks)", i, this->to_microseconds_(item[i].duration1),
                item[i].duration1);
    }
  }
  ESP_LOGVV(TAG, "\n");

  this->temp_.reserve(item_count * 2);  // each RMT item has 2 pulses
  for (size_t i = 0; i < item_count; i++) {
    if (item[i].duration0 == 0u) {
      // Do nothing
    } else if ((bool(item[i].level0) == prev_level) || (item[i].duration0 < filter_ticks)) {
      prev_length += item[i].duration0;
    } else {
      if (prev_length > 0) {
        if (prev_level) {
          this->temp_.push_back(this->to_microseconds_(prev_length) * multiplier);
        } else {
          this->temp_.push_back(-int32_t(this->to_microseconds_(prev_length)) * multiplier);
        }
      }
      prev_level = bool(item[i].level0);
      prev_length = item[i].duration0;
    }

    if (item[i].duration1 == 0u) {
      // Do nothing
    } else if ((bool(item[i].level1) == prev_level) || (item[i].duration1 < filter_ticks)) {
      prev_length += item[i].duration1;
    } else {
      if (prev_length > 0) {
        if (prev_level) {
          this->temp_.push_back(this->to_microseconds_(prev_length) * multiplier);
        } else {
          this->temp_.push_back(-int32_t(this->to_microseconds_(prev_length)) * multiplier);
        }
      }
      prev_level = bool(item[i].level1);
      prev_length = item[i].duration1;
    }
  }
  if (prev_length > 0) {
    if (prev_level) {
      this->temp_.push_back(this->to_microseconds_(prev_length) * multiplier);
    } else {
      this->temp_.push_back(-int32_t(this->to_microseconds_(prev_length)) * multiplier);
    }
  }
}

}  // namespace remote_receiver
}  // namespace esphome

#endif
