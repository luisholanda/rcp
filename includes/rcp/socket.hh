#pragma once

#include "rcp/packet.hh"
#include <memory>
#include <vector>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

static int udp_socket(int domain, int type, int protocol) {
  return socket(domain, type, protocol);
}

namespace rcp {

class connection;
class listener;

class socket {
public:
  socket() {
    if ((mUdpSocket = udp_socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
      throw "failed to create base UDP socket";
    }
  }

  listener Bind(in_port_t port);
  connection Connect(in_addr_t ip, in_port_t port);
protected:
  int Recv(packet& packet, sockaddr& peer) noexcept;
  int Send(const packet& packet, const sockaddr& peer) noexcept;

  ~socket() {
    close(mUdpSocket);
  }

  friend class connection;
private:
  std::unique_ptr<packet::buffer_t> AcquireBuffer() noexcept;
  void ReleaseBuffer(std::unique_ptr<packet::buffer_t> buff) noexcept;

  int mUdpSocket;
  std::vector<std::unique_ptr<packet::buffer_t>> mBuffers;
};
} // end namespace rpc
