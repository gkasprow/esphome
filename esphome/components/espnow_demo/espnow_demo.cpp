#include "espnow_demo.h"

#pragma once

#if defined(USE_ESP32)

#include "esphome/core/automation.h"
#include "esphome/core/component.h"
#include "esphome/core/helpers.h"
#include <esp_now.h>

#include <array>
#include <memory>
#include <queue>
#include <vector>
#include <mutex>
#include <map>

namespace esphome {
namespace espnow {}  // namespace espnow
}  // namespace esphome

#endif
