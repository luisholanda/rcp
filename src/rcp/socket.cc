#include "rcp/connection.hh"
#include "rcp/listener.hh"
#include "rcp/packet.hh"
#include "rcp/socket.hh"
#include <cerrno>

using namespace rcp;

listener socket::Bind(in_port_t port) {
  sockaddr_in anyaddr;
  std::memset(&anyaddr, 0, sizeof(anyaddr));

  anyaddr.sin_family = AF_INET;
  anyaddr.sin_addr.s_addr = INADDR_ANY;
  anyaddr.sin_port = port;

  if (bind(mUdpSocket,
           reinterpret_cast<const sockaddr*>(&anyaddr),
           sizeof(anyaddr)) < 0)
  {
    // TODO: Someday we should handle these errno with custom exceptions.
    throw errno;
  }

  return listener(*this, port);
}

connection socket::Connect(in_addr_t ip, in_port_t port) {
  sockaddr_in peerAddr;
  std::memset(&peerAddr, 0, sizeof(peerAddr));

  return connection(*this, peerAddr);
}

int socket::Recv(packet& packet, sockaddr& peer) noexcept {
  auto buff = AcquireBuffer();

  socklen_t len = sizeof(peer);
  auto n = recvfrom(mUdpSocket,
                    static_cast<void*>(&*buff),
                    packet::PACKET_SIZE,
                    MSG_WAITALL,
                    &peer,
                    &len);
  auto err = errno;

  packet.FromBuffer(*buff, n);
  ReleaseBuffer(std::move(buff));
  return err;
}

int socket::Send(const packet& packet, const sockaddr& peer) noexcept {
  sendto(mUdpSocket, packet.begin(), packet::PACKET_SIZE,
         0, &peer, 1);
  return errno;
}

std::unique_ptr<packet::buffer_t> socket::AcquireBuffer() noexcept {
  if (mBuffers.empty()) {
    return packet::makeBuffer();
  } else {
    auto buff = std::move(mBuffers.back());
    mBuffers.pop_back();
    return std::move(buff);
  }
}

inline void socket::ReleaseBuffer(std::unique_ptr<packet::buffer_t> buffer) noexcept {
  mBuffers.push_back(std::move(buffer));
}
