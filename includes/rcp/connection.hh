#include <cstdint>
#include <netinet/in.h>
#include <vector>

namespace rcp {
class connection;
class packet;
class socket;

class __cong_algo {
protected:
  virtual bool CanSendPacket() const noexcept;
  virtual void PacketSent() noexcept;
  virtual void AckRecv(bool repeated) noexcept;
  virtual bool ResendFirstUnackedPacket() const noexcept;
  virtual void ResetAckTimeout() noexcept;

  friend class connection;
};

class __cong_wndw {
protected:
  using packets_t = std::vector<std::unique_ptr<packet>>;
  friend class connection;
private:
  packets_t mPndgPackets;
  packets_t mBuffPackets;
};

class connection {
protected:
  connection(socket& socket, sockaddr_in peer) noexcept
    : mSocket(socket)
    , mPeer(peer)
  {}

  friend class socket;
protected:
  void Handshake();
private:
  socket& mSocket;
  std::uint16_t mNextSeqNum;
  std::uint16_t mNextAckNum;
  const sockaddr_in mPeer;
  __cong_algo mCongAlgo;
  __cong_wndw mCongWndw;
};
}
