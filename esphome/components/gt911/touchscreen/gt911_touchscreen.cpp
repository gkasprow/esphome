#include "gt911_touchscreen.h"

#include "esphome/core/helpers.h"
#include "esphome/core/log.h"

namespace esphome {
namespace gt911 {

static const char *const TAG = "gt911.touchscreen";

static const uint8_t GET_TOUCH_STATE[2] = {0x81, 0x4E};
static const uint8_t CLEAR_TOUCH_STATE[3] = {0x81, 0x4E, 0x00};
static const uint8_t GET_TOUCHES[2] = {0x81, 0x4F};
static const uint8_t GET_SWITCHES[2] = {0x80, 0x4D};
static const uint8_t GET_MAX_VALUES[2] = {0x80, 0x48};
static const size_t MAX_TOUCHES = 5;  // max number of possible touches reported
static const size_t MAX_BUTTONS = 4;  // max number of buttons scanned

#define ERROR_CHECK(err) \
  if ((err) != i2c::ERROR_OK) { \
    ESP_LOGE(TAG, "Failed to communicate!"); \
    this->status_set_warning(); \
    return; \
  }

void GT911Touchscreen::setup() {
  i2c::ErrorCode err;
  ESP_LOGCONFIG(TAG, "Setting up GT911 Touchscreen...");

  this->setup_noise_level(this->noise_level_);

  if (this->reset_pin_ != nullptr) {
    this->reset_pin_->setup();
    this->reset_pin_->digital_write(false);
    if (this->interrupt_pin_ != nullptr) {
      // The interrupt pin is used as an input during reset to select the I2C address.
      this->interrupt_pin_->pin_mode(gpio::FLAG_OUTPUT);
      this->interrupt_pin_->setup();
      this->interrupt_pin_->digital_write(false);
    }
    delay(2);
    this->reset_pin_->digital_write(true);
    delay(50);  // NOLINT
    if (this->interrupt_pin_ != nullptr) {
      this->interrupt_pin_->pin_mode(gpio::FLAG_INPUT);
      this->interrupt_pin_->setup();
    }
  }

  // check the configuration of the int line.
  uint8_t data[4];
  err = this->write(GET_SWITCHES, 2);
  if (err == i2c::ERROR_OK) {
    err = this->read(data, 1);
    if (err == i2c::ERROR_OK) {
      ESP_LOGD(TAG, "Read from switches: 0x%02X", data[0]);
      if (this->interrupt_pin_ != nullptr) {
        // datasheet says NOT to use pullup/down on the int line.
        this->interrupt_pin_->pin_mode(gpio::FLAG_INPUT);
        this->interrupt_pin_->setup();
        this->attach_interrupt_(this->interrupt_pin_,
                                (data[0] & 1) ? gpio::INTERRUPT_FALLING_EDGE : gpio::INTERRUPT_RISING_EDGE);
      }
    }
  }
  if (err == i2c::ERROR_OK) {
    err = this->write(GET_MAX_VALUES, 2);
    if (err == i2c::ERROR_OK) {
      err = this->read(data, sizeof(data));
      if (err == i2c::ERROR_OK) {
        if (this->x_raw_max_ == this->x_raw_min_) {
          this->x_raw_max_ = encode_uint16(data[1], data[0]);
        }
        if (this->y_raw_max_ == this->y_raw_min_) {
          this->y_raw_max_ = encode_uint16(data[3], data[2]);
        }
        esph_log_d(TAG, "calibration max_x/max_y %d/%d", this->x_raw_max_, this->y_raw_max_);
      }
    }
  }
  if (err != i2c::ERROR_OK) {
    ESP_LOGE(TAG, "Failed to communicate!");
    this->mark_failed();
    return;
  }

  ESP_LOGCONFIG(TAG, "GT911 Touchscreen setup complete");
}

uint8_t GT911Touchscreen::gt911_checksum(uint8_t *buf, uint8_t len) {
  uint8_t checksum = 0;
  for (uint8_t i = 0; i < len; i++) {
    checksum += buf[i];
  }
  checksum = (~checksum) + 1;
  return checksum;
}

#define GT_REG_CONFIG 0x8047
#define GT_CONFIG_FRESH 0x8100
void GT911Touchscreen::setup_noise_level(int nr_level) {
  uint8_t len = GT_CONFIG_FRESH - GT_REG_CONFIG;
  uint8_t cfg[len];
  uint8_t err;
  uint8_t done = 1;

  // Check noise_level is within limits
  if (nr_level < 0 || nr_level > 15) {
    ESP_LOGD(TAG, "Noise Level value out of range (0-15)");
    return;
  }

  memset(cfg, 0, len);

  // Split the read down as it appears reading 185 bytes in one
  // go is too much
  err = this->read_register16(GT_REG_CONFIG, cfg, 100);
  if (err != i2c::ERROR_OK) {
    ESP_LOGD(TAG, "Failed to read config space (1)");
    return;
  }

  err = this->read_register16(GT_REG_CONFIG + 100, cfg + 100, len - 100);
  if (err != i2c::ERROR_OK) {
    ESP_LOGD(TAG, "Failed to read config space (2)");
    return;
  }

  if (cfg[len - 1] != this->gt911_checksum(cfg, len - 1)) {
    ESP_LOGD(TAG, "Failed to read config space (checksum failed)");
    return;
  }

  uint8_t old_level = cfg[11];

  if (cfg[11] == (uint8_t) nr_level) {
    ESP_LOGD(TAG, "noise reduction unchanged");
    return;
  }

  cfg[11] = (uint8_t) nr_level;
  cfg[len - 1] = this->gt911_checksum(cfg, len - 1);

  // Split the read down as it appears writing 185 bytes in one
  // go is also too much
  err = this->write_register16(GT_REG_CONFIG, cfg, 100);
  if (err != i2c::ERROR_OK) {
    ESP_LOGD(TAG, "Failed to write config space (1)");
    return;
  }
  err = this->write_register16(GT_REG_CONFIG + 100, cfg + 100, len - 100);
  if (err != i2c::ERROR_OK) {
    ESP_LOGD(TAG, "Failed to write config space (2)");
    return;
  }

  err = this->write_register16(GT_CONFIG_FRESH, &done, 1);
  if (err != i2c::ERROR_OK) {
    ESP_LOGW(TAG, "Failed to update GT911 configuration");
    return;
  }

  ESP_LOGD(TAG, "noise level updated from %i to %i", old_level, nr_level);
  return;
}

void GT911Touchscreen::update_touches() {
  i2c::ErrorCode err;
  uint8_t touch_state = 0;
  uint8_t data[MAX_TOUCHES + 1][8];  // 8 bytes each for each point, plus extra space for the key byte

  err = this->write(GET_TOUCH_STATE, sizeof(GET_TOUCH_STATE), false);
  ERROR_CHECK(err);
  err = this->read(&touch_state, 1);
  ERROR_CHECK(err);
  this->write(CLEAR_TOUCH_STATE, sizeof(CLEAR_TOUCH_STATE));
  uint8_t num_of_touches = touch_state & 0x07;

  if ((touch_state & 0x80) == 0 || num_of_touches > MAX_TOUCHES) {
    this->skip_update_ = true;  // skip send touch events, touchscreen is not ready yet.
    return;
  }

  err = this->write(GET_TOUCHES, sizeof(GET_TOUCHES), false);
  ERROR_CHECK(err);
  // num_of_touches is guaranteed to be 0..5. Also read the key data
  err = this->read(data[0], sizeof(data[0]) * num_of_touches + 1);
  ERROR_CHECK(err);

  for (uint8_t i = 0; i != num_of_touches; i++) {
    uint16_t id = data[i][0];
    uint16_t x = encode_uint16(data[i][2], data[i][1]);
    uint16_t y = encode_uint16(data[i][4], data[i][3]);
    this->add_raw_touch_position_(id, x, y);
  }
  auto keys = data[num_of_touches][0] & ((1 << MAX_BUTTONS) - 1);
  if (keys != this->button_state_) {
    this->button_state_ = keys;
    for (size_t i = 0; i != MAX_BUTTONS; i++) {
      for (auto *listener : this->button_listeners_)
        listener->update_button(i, (keys & (1 << i)) != 0);
    }
  }
}

void GT911Touchscreen::dump_config() {
  ESP_LOGCONFIG(TAG, "GT911 Touchscreen:");
  LOG_I2C_DEVICE(this);
  LOG_PIN("  Interrupt Pin: ", this->interrupt_pin_);
  ESP_LOGI(TAG, "  GT911 noise level: %i", this->noise_level_);
}

}  // namespace gt911
}  // namespace esphome
