#include "project_name_text_sensor.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"
#include "esphome/core/version.h"

namespace esphome {
namespace project_name {

static const char *const TAG = "project_name.text_sensor";

void ProjectNameTextSensor::setup() { this->publish_state(ESPHOME_PROJECT_NAME); }
float ProjectNameTextSensor::get_setup_priority() const { return setup_priority::DATA; }
std::string ProjectNameTextSensor::unique_id() { return get_mac_address() + "-project_name"; }
void ProjectNameTextSensor::dump_config() { LOG_TEXT_SENSOR("", "Project Name Text Sensor", this); }

}  // namespace project_name
}  // namespace esphome
