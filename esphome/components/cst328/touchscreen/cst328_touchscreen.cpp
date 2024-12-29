#include "cst328_touchscreen.h"

namespace esphome {
namespace cst328 {

void CST328Touchscreen::setup() {
  ESP_LOGCONFIG(TAG, "Setting up CST328 Touchscreen...");
  if (this->reset_pin_ != nullptr) {
    this->reset_pin_->setup();
    this->reset_pin_->digital_write(true);
    delay(50);
    this->reset_pin_->digital_write(false);
    delay(5);
    this->reset_pin_->digital_write(true);
    this->set_timeout(50, [this] { this->continue_setup_(); });
  } else {
    this->continue_setup_();
  }
}

void CST328Touchscreen::continue_setup_() {
  uint8_t buffer[4];

  if (this->interrupt_pin_ != nullptr) {
    this->interrupt_pin_->setup();
    this->attach_interrupt_(this->interrupt_pin_, gpio::INTERRUPT_FALLING_EDGE);
  }

  // buffer[0] = 0xD1;
  // if (this->write_register16(0xD1, buffer, 1) != i2c::ERROR_OK) {
  //   ESP_LOGE(TAG, "Write byte to 0xD1 failed");
  //   this->mark_failed();
  //   return;
  // }
  // delay(10);

  // Enter debug/info mode
  if (this->write_register16(static_cast<uint16_t>(Cst328WorkModes::DEBUG_INFO_MODE), buffer, 0) != i2c::ERROR_OK) {
    ESP_LOGE(TAG, "Failed to enter debug/info mode");
    this->mark_failed();
    return;
  }

  // Read chip and project ID
  if (this->read16_(static_cast<u_int16_t>(Cst328Registers::CHIP_TYPE_AND_PROJECT_ID), buffer, 4)) {
    this->chip_id_ = buffer[2] + (buffer[3] << 8);
    this->project_id_ = buffer[0] + (buffer[1] << 8);
    ESP_LOGV(TAG, "Chip ID %X, project ID %X", this->chip_id_, this->project_id_);
  }

  // Read FW checksum
  if (this->read16_(static_cast<uint16_t>(Cst328Registers::FW_CRC_AND_BOOT_TIME), buffer, 4)) {
    uint16_t fw_crc = buffer[2] + (buffer[3] << 8);
    if (0xCACA != fw_crc) {
      ESP_LOGE(TAG, "Invalid FW checksum: %X", fw_crc);
      this->mark_failed();
      return;
    }
  }

  // Read FW version
  if (this->read16_(static_cast<u_int16_t>(Cst328Registers::FW_REVISION), buffer, 4)) {
    this->fw_ver_major_ = buffer[3];
    this->fw_ver_minor_ = buffer[2];
    this->fw_build_ = buffer[0] + (buffer[1] << 8);
    ESP_LOGV(TAG, "FW version %d.%d.%d", this->fw_ver_major_, this->fw_ver_minor_, this->fw_build_);
  }

  // Read X/Y resolution
  if (this->read16_(static_cast<u_int16_t>(Cst328Registers::X_Y_RESOLUTION), buffer, 4)) {
    this->x_raw_max_ = buffer[0] + (buffer[1] << 8);
    this->y_raw_max_ = buffer[2] + (buffer[3] << 8);
  } else {
    this->x_raw_max_ = this->display_->get_native_width();
    this->y_raw_max_ = this->display_->get_native_height();
  }

  // Enter normal mode
  this->write_register16(static_cast<uint16_t>(Cst328WorkModes::NORMAL_MODE), buffer, 0);

  // read once and sync?
  uint8_t sync_byte;
  this->read_register16(static_cast<u_int16_t>(Cst328Registers::TOUCH_INFORMATION), &sync_byte, 1);
  sync_byte = 0xAB;
  this->write_register16(static_cast<u_int16_t>(Cst328Registers::TOUCH_INFORMATION), &sync_byte, 1);

  this->setup_complete_ = true;

  ESP_LOGV(TAG, "CST328 Touchscreen setup complete");
}

void CST328Touchscreen::dump_config() {
  ESP_LOGCONFIG(TAG, "CST328 Touchscreen:");
  LOG_I2C_DEVICE(this);
  LOG_PIN("  Interrupt Pin: ", this->interrupt_pin_);
  LOG_PIN("  Reset Pin: ", this->reset_pin_);
  ESP_LOGCONFIG(TAG, "  Chip ID: 0x%04X, Project ID: 0x%04X", this->chip_id_, this->project_id_);
  ESP_LOGCONFIG(TAG, "  FW version: %d.%d.%d", this->fw_ver_major_, this->fw_ver_minor_, this->fw_build_);
  ESP_LOGCONFIG(TAG, "  X/Y resolution: %d/%d", this->x_raw_max_, this->y_raw_max_);
}

void CST328Touchscreen::update_touches() {
  ESP_LOGV(TAG, "update_touches()...");
  uint8_t data[CST328_TOUCH_DATA_SIZE];
  uint8_t clear{0};
  uint8_t touch_cnt = 0;
  this->skip_update_ = true;
  auto err = this->read_register16(static_cast<u_int16_t>(Cst328Registers::TOUCH_FINGER_NUMBER), data, 1);
  if (err != i2c::ERROR_OK) {
    ESP_LOGV(TAG, "update_touches() cant read touch count");
    this->status_set_warning();
    return;
  }
  this->status_clear_warning();

  // number of fingers
  touch_cnt = data[0] & 0x0F;

  // no touch or error
  if (touch_cnt == 0 || touch_cnt > CST328_TOUCH_MAX_POINTS) {
    // clear touch
    ESP_LOGV(TAG, "update_touches() no touch or error (count=%d)", touch_cnt);
    this->write_register16(static_cast<u_int16_t>(Cst328Registers::TOUCH_FINGER_NUMBER), &clear, 1);
    return;
  }
  ESP_LOGV(TAG, "update_touches() touch count=%d", touch_cnt);

  // read all points
  err = this->read_register16(static_cast<u_int16_t>(Cst328Registers::TOUCH_INFORMATION), data, sizeof(data));
  if (err != i2c::ERROR_OK) {
    ESP_LOGV(TAG, "Failed to read touch data");
    this->status_set_warning();
    return;
  }

  // clear touch
  //  this->writeRegister(MODE_NORMAL_0_REG, (uint8_t)0xAB); // sync signal ?
  clear = 0xAB;
  this->write_register16(static_cast<u_int16_t>(Cst328Registers::TOUCH_INFORMATION), &clear, 1);

  this->skip_update_ = false;
  size_t index = 0;
  for (uint8_t i = 0; i != touch_cnt; i++) {
    uint8_t id = data[index] >> 4;
    int16_t x = (data[index + 1] << 4) | ((data[index + 3] >> 4) & 0x0F);
    int16_t y = (data[index + 2] << 4) | (data[index + 3] & 0x0F);
    int16_t z = data[index + 4];
    this->add_raw_touch_position_(id, x, y, z);
    ESP_LOGV(TAG, "Read touch %d: %d/%d", id, x, y);
    index += 5;
    if (i == 0) {
      index += 2;
    }
  }
  ESP_LOGV(TAG, "update_touches() done - normal quit");
}
}  // namespace cst328
}  // namespace esphome
