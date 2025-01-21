#include "bmp_image.h"

#ifdef USE_ONLINE_IMAGE_BMP_SUPPORT

#include "esphome/components/display/display.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"

namespace esphome {
namespace online_image {

static const char *const TAG = "online_image.bmp";

int HOT BmpDecoder::decode(uint8_t *buffer, size_t size) {
  size_t index = 0;
  this->decoded_bytes_ += size;
  if (this->current_index_ == 0 && this->decoded_bytes_ > 14) {
    // Check if the file is a BMP image
    if (buffer[0] != 'B' || buffer[1] != 'M') {
      ESP_LOGE(TAG, "Not a BMP file");
      return 0;
    }

    this->total_size_ = encode_uint32(buffer[5], buffer[4], buffer[3], buffer[2]);
    this->data_offset_ = encode_uint32(buffer[13], buffer[12], buffer[11], buffer[10]);

    this->current_index_ = 14;
    index = 14;
  }
  if (this->current_index_ == 14 && this->decoded_bytes_ > data_offset_) {
    this->width_ = encode_uint32(buffer[21], buffer[20], buffer[19], buffer[18]);
    this->height_ = encode_uint32(buffer[25], buffer[24], buffer[23], buffer[22]);
    this->bitsPerPixel_ = encode_uint16(buffer[29], buffer[28]);
    this->compressionMethod_ = encode_uint32(buffer[33], buffer[32], buffer[31], buffer[30]);
    this->imageDataSize_ = encode_uint32(buffer[37], buffer[36], buffer[35], buffer[34]);
    this->colorTableEntries_ = encode_uint32(buffer[49], buffer[48], buffer[47], buffer[46]);

    switch (this->bitsPerPixel_) {
      case 1:
        this->widthBytes_ = (this->width_ % 8 == 0) ? (this->width_ / 8) : (this->width_ / 8 + 1);
        break;
      default:
        ESP_LOGE(TAG, "Unsupported bits per pixel: %d", this->bitsPerPixel_);
        return 0;
    }

    if (!this->set_size(this->width_, this->height_)) {
      return 0;
    }
    this->current_index_ = this->data_offset_;
    index = this->data_offset_;
  }
  while (this->current_index_ < this->decoded_bytes_) {
    size_t paint_index = this->current_index_ - this->data_offset_;

    uint8_t current_byte = buffer[index];
    for (uint8_t i = 0; i < 8; i++) {
      size_t x = (paint_index * 8) % this->width_ + i;
      size_t y = (this->height_ - 1) - (paint_index / this->widthBytes_);
      Color c = (current_byte & (1 << (7 - i))) ? display::COLOR_ON : display::COLOR_OFF;
      this->draw(x, y, 1, 1, c);
    }
    this->current_index_++;
    index++;
  }
  return size;
};

}  // namespace online_image
}  // namespace esphome

#endif  // USE_ONLINE_IMAGE_BMP_SUPPORT
