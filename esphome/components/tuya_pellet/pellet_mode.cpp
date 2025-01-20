#include "pellet_mode.h"

namespace esphome {
namespace climate {

const LogString *pellet_eco_mode_to_string(PelletEcoMode mode) {
  switch (mode) {
    case CLIMATE_ECO1:
      return LOG_STR("ECO ON");
    case CLIMATE_ECO2:
      return LOG_STR("ECO OFF");
    default:
      return LOG_STR("UNKNOWN");
  }
}
const LogString *climate_heat_level_to_string(PelletHeatLevel level) {
  switch (level) {
    case CLIMATE_HEAT1:
      return LOG_STR("POWER LEVEL 4");
    case CLIMATE_HEAT2:
      return LOG_STR("POWER LEVEL 4");
    case CLIMATE_HEAT3:
      return LOG_STR("POWER LEVEL 2");
    case CLIMATE_HEAT4:
      return LOG_STR("POWER LEVEL 1");
    default:
      return LOG_STR("UNKNOWN");
  }
}

}  // namespace climate
}  // namespace esphome
