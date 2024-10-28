#pragma once

#include <cstdint>
#include <stddef.h>
#include "esphome/core/helpers.h"

namespace esphome {
namespace audio {

#ifndef USE_ESP32
using TickType_t = size_t;
#endif

enum State : uint8_t {
  STATE_STOPPED = 0,
  STATE_STARTING,
  STATE_RUNNING,
  STATE_STOPPING,
};

struct AudioStreamInfo {
  bool operator==(const AudioStreamInfo &rhs) const {
    return (channels == rhs.channels) && (bits_per_sample == rhs.bits_per_sample) && (sample_rate == rhs.sample_rate);
  }
  bool operator!=(const AudioStreamInfo &rhs) const { return !operator==(rhs); }
  size_t get_bytes_per_sample() const { return bits_per_sample / 8; }
  uint8_t channels = 1;
  uint8_t bits_per_sample = 16;
  uint32_t sample_rate = 16000;
};

class AudioListener;

class AudioStreamer : public Parented<AudioListener> {
 public:
  virtual ~AudioStreamer();

  /// @brief Plays the provided audio data or receive the audio from the mic.
  /// @param length The length of the audio data in bytes.
  /// @return The number of bytes that were actually written to the speaker's internal buffer.

  size_t stream(const uint8_t *data, const size_t size, TickType_t ticks_to_wait = 0);
  bool is_running();
};

class AudioListener {
 public:
  /// @brief Initialize the audio device
  /// If the audio stream is not the default defined in "esphome/core/audio.h"
  /// and the speaker component implements it,
  /// then this should be called after calling ``set_audio_stream_info``.
  /// @param data Audio data in the format specified by ``set_audio_stream_info`` method.
  /// @return the AudioStreamer object to be used to stream to or from the device.
  AudioStreamer *start(const AudioStreamInfo &audio_stream_info);
  AudioStreamer *start();

  void stop() {
    if (this->current_streamer_ != nullptr) {
      delete this->current_streamer_;
    }
  }

  virtual bool can_stream(AudioStreamer *streamer);

  bool is_running() const { return this->state_ == audio::STATE_RUNNING; }
  bool is_stopped() const { return this->state_ == audio::STATE_STOPPED; }

  void set_audio_stream_info(const AudioStreamInfo &audio_stream_info) { this->audio_stream_info_ = audio_stream_info; }
  virtual void get_default_audio_stream_info(AudioStreamInfo &audio_stream_info) {}

 protected:
  virtual bool starting(const AudioStreamInfo &audio_stream_info) = 0;
  virtual size_t streaming(const uint8_t *data, size_t size, TickType_t ticks_to_wait) = 0;
  virtual void stopping(){};

  AudioStreamer *current_streamer_{nullptr};
  audio::AudioStreamInfo audio_stream_info_;
  State state_{STATE_STOPPED};
  bool finish_before_stop_{false};
};

}  // namespace audio
}  // namespace esphome
