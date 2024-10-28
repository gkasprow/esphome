#include "audio.h"

namespace esphome {
namespace audio {

/* *************** AudioListener **************** */

AudioStreamer *AudioListener::start(AudioStreamInfo &info) {
  if (current_streamer_ != nullptr) {
    return nullptr;
  }
  if (this->starting(info)) {
    this->current_streamer_ = new AudioStreamer();
    this->current_streamer_->set_parent(this);
  }
  return nullptr;
}

AudioStreamer *AudioListener::start() {
  AudioStreamInfo info;
  this->get_default_audio_stream_info(info);
  this->start(info);
}

bool AudioListener::can_stream(AudioStreamer *streamer) {
  return this->current_streamer_ == streamer && this->is_running();
}

/* *************** AudioStreamer **************** */

AudioStreamer::~AudioStreamer() {
  if (this->parent_ != nullptr && this->parent_->current_streamer_ == this) {
    this->parent_->current_streamer_ = nullptr;
    this->parent_->stopping();
    this->parent_ = nullptr;
  }
}

bool AudioStreamer::is_running() { return (this->parent_ == nullptr) ? false : this->parent_->can_stream(this); }

size_t AudioStreamer::stream(const uint8_t *data, const size_t size, TickType_t ticks_to_wait) {
  if (!this->is_running(this))
    return 0;
  return this->parent_->streaming(data, size, ticks_to_wait);
}

}  // namespace audio
}  // namespace esphome
