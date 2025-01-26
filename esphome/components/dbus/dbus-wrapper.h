#pragma once

#include <string>
#include <list>
#include <dbus/dbus.h>

#include "esphome/core/defines.h"

namespace esphome {
namespace dbus {

std::string dbusIterToString(DBusMessageIter *iter);

// class DBus : public EntityBase {
class DBusWrapper {
 public:
  DBusWrapper(bool dbus_system = false) : system(dbus_system){};
  ~DBusWrapper();
  void setup();

  //  void loop() override;

  std::string send(const std::string &dbus_destination, const std::string &dbus_path, const std::string &dbus_interface,
                   const std::string &dbus_method, const std::list<std::string> &dbus_args,
                   const std::list<std::string> &properties = {}, const std::string &property_separator = "");

  std::string getProperty(DBusMessage *msg, const std::string &searchKey);

  /*
    // für die Action
    void send(const optional<std::string> &dbus_destination, const optional<std::string> &dbus_path,
      const optional<std::string> &dbus_interface,
      const optional<std::string> &dbus_method,
    const optional<std::string> &dbus_properties,
    const optional<std::string> &dbus_args);
  */

  void set_system(bool system) { this->system = system; };

 protected:
  DBusConnection *conn{NULL};
  void registerForSignal(const std::string &dbus_properties, const std::string &dbus_path);
  bool system = false;
};

}  // namespace dbus
}  // namespace esphome
