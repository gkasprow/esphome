#pragma once

#include "esphome/core/component.h"
#include "esphome/components/packet_encoding/packet_encoding.h"
#include "esphome/components/network/ip_address.h"
#if defined(USE_SOCKET_IMPL_BSD_SOCKETS) || defined(USE_SOCKET_IMPL_LWIP_SOCKETS)
#include "esphome/components/socket/socket.h"
#endif
#ifdef USE_SOCKET_IMPL_LWIP_TCP
#include <WiFiUdp.h>
#endif
#include <vector>
#include <map>

namespace esphome {
namespace udp {

static const size_t MAX_PACKET_SIZE = 508;
class UDPComponent : public packet_encoding::PacketEncoding {
 public:
  void setup() override;
  void loop() override;
  void update() override;
  void dump_config() override;

  void add_address(const char *addr) { this->addresses_.emplace_back(addr); }
  void set_listen_address(const char *listen_addr) { this->listen_address_ = network::IPAddress(listen_addr); }
  void set_port(uint16_t port) { this->port_ = port; }
  float get_setup_priority() const override { return setup_priority::AFTER_WIFI; }

 protected:
  bool should_send_() override;
  size_t get_max_packet_size_() override { return MAX_PACKET_SIZE; }
  uint16_t port_{18511};
  bool should_broadcast_{};
  bool should_listen_{};

#if defined(USE_SOCKET_IMPL_BSD_SOCKETS) || defined(USE_SOCKET_IMPL_LWIP_SOCKETS)
  std::unique_ptr<socket::Socket> broadcast_socket_ = nullptr;
  std::unique_ptr<socket::Socket> listen_socket_ = nullptr;
  std::vector<struct sockaddr> sockaddrs_{};
#endif
#ifdef USE_SOCKET_IMPL_LWIP_TCP
  std::vector<IPAddress> ipaddrs_{};
  WiFiUDP udp_client_{};
#endif
  std::vector<uint8_t> encryption_key_{};
  std::vector<std::string> addresses_{};

  optional<network::IPAddress> listen_address_{};
  void send_packet_(void *data, size_t len) override;
};

}  // namespace udp
}  // namespace esphome
