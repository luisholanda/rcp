#include "rcp/packet.hh"
#include "rcp/socket.hh"
#include <cerrno>

using namespace rcp;

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

void socket::ReleaseBuffer(std::unique_ptr<packet::buffer_t> buffer) noexcept {
  mBuffers.push_back(std::move(buffer));
}
