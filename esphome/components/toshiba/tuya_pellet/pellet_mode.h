#pragma once

#include <cstdint>
#include "esphome/core/log.h"

namespace esphome {
namespace climate {

/// Enum for all eco modes the pellet stove can be in.
enum PelletEcoMode : uint8_t {
  /// The climate device is set to not maintain flame in fire box.
  CLIMATE_ECO1 = 0,
  /// The climate device is set to maintain flame in firebox in order to reduce time to return to set point after sitting idle
  CLIMATE_ECO2 = 1,

};

/// Enum for the all heat levels available for pellet stove
enum PelletHeatLevel : uint8_t {

  CLIMATE_HEAT1 = 0,

  CLIMATE_HEAT2 = 1,

  CLIMATE_HEAT3 = 2,

  CLIMATE_HEAT4 = 3,

};

/// Convert the given PelletEcoMode to a human-readable string.
const LogString *pellet_eco_mode_to_string(PelletEcoMode mode);

/// Convert the given PelletHeatLevel to a human-readable string.
const LogString *climate_heat_level_to_string(PelletHeatLevel level);


}  // namespace climate
}  // namespace esphome
