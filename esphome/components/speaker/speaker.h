#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#ifdef USE_ESP32
#include <freertos/FreeRTOS.h>
#endif

#include "esphome/core/defines.h"

#include "esphome/components/audio/audio.h"
#ifdef USE_AUDIO_DAC
#include "esphome/components/audio_dac/audio_dac.h"
#endif

namespace esphome {
namespace speaker {

using AudioStreamer = audio::AudioStreamer;
using State = audio::State;

class Speaker : public audio::AudioListener {
 public:
  virtual bool has_buffered_data() const = 0;

  // Volume control is handled by a configured audio dac component. Individual speaker components can
  // override and implement in software if an audio dac isn't available.
  virtual void set_volume(float volume) {
    this->volume_ = volume;
#ifdef USE_AUDIO_DAC
    if (this->audio_dac_ != nullptr) {
      this->audio_dac_->set_volume(volume);
    }
#endif
  };
  float get_volume() { return this->volume_; }

  virtual void set_mute_state(bool mute_state) {
    this->mute_state_ = mute_state;
#ifdef USE_AUDIO_DAC
    if (this->audio_dac_) {
      if (mute_state) {
        this->audio_dac_->set_mute_on();
      } else {
        this->audio_dac_->set_mute_off();
      }
    }
#endif
  }
  bool get_mute_state() { return this->mute_state_; }

#ifdef USE_AUDIO_DAC
  void set_audio_dac(audio_dac::AudioDac *audio_dac) { this->audio_dac_ = audio_dac; }
#endif

 protected:
  float volume_{1.0f};
  bool mute_state_{false};

#ifdef USE_AUDIO_DAC
  audio_dac::AudioDac *audio_dac_{nullptr};
#endif
};

}  // namespace speaker
}  // namespace esphome
