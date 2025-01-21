#pragma once

#include "esphome/core/defines.h"
#ifdef USE_ONLINE_IMAGE_BMP_SUPPORT

#include "image_decoder.h"

namespace esphome {
namespace online_image {

/**
 * @brief Image decoder specialization for PNG images.
 */
class BmpDecoder : public ImageDecoder {
 public:
  /**
   * @brief Construct a new PNG Decoder object.
   *
   * @param display The image to decode the stream into.
   */
  BmpDecoder(OnlineImage *image) : ImageDecoder(image) {}

  int HOT decode(uint8_t *buffer, size_t size) override;

 protected:
  size_t current_index_{0};
  ssize_t width_{0};
  ssize_t height_{0};
  uint16_t bitsPerPixel_{0};
  uint32_t compressionMethod_{0};
  uint32_t imageDataSize_{0};
  uint32_t colorTableEntries_{0};
  size_t widthBytes_{0};
  size_t total_size_{0};
  size_t data_offset_{0};
};

}  // namespace online_image
}  // namespace esphome

#endif  // USE_ONLINE_IMAGE_BMP_SUPPORT
