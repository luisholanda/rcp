#include <rcp/packet.hh>

using namespace rcp;

void packet::SetSequenceNumber(std::uint16_t seqNum) {
  if (seqNum < MAX_SEQ_NUM) {
    mSeqNum = seqNum;
  } else {
    throw "invalid sequence number";
  }
}

void packet::SetAcknowledgmentNumber(std::uint16_t ackNum) {
  if (ackNum < MAX_SEQ_NUM) {
    mAckNum = ackNum;
  } else {
    throw "invalid acknowledgment number";
  }
}

void packet::SetLength(std::uint16_t length) {
  if (length <= MAX_PKT_PAYLOAD_SIZE) {
    mLength = length;
  } else {
    throw "invalid payload length";
  }
}

void packet::Clear() noexcept {
  mLength = 0;
  mFlags = 0;
  mSeqNum = 0;
  mAckNum = 0;
}

std::unique_ptr<packet> packet::make() noexcept {
  return std::make_unique<packet>();
}

std::unique_ptr<packet>
packet::fromBuffer(std::unique_ptr<packet::buffer_t> buffer, std::size_t n) noexcept {
  if (n < HEADER_SIZE || n > PACKET_SIZE) {
    return nullptr;
  }

  auto packetPtr = reinterpret_cast<packet*>(buffer.release());
  std::unique_ptr<packet> packet(packetPtr);
  packet->SetLength(n - HEADER_SIZE);

  return std::move(packet);
}

void packet::FromBuffer(const packet::buffer_t& buffer, std::size_t n) noexcept {
  if (n < HEADER_SIZE || n > PACKET_SIZE) {
    return;
  }

  std::uint16_t seqNum, ackNum;

  std::memcpy(&seqNum, buffer, 2);
  std::memcpy(&ackNum, buffer + sizeof(std::uint16_t), 2);

  if (seqNum > MAX_SEQ_NUM || ackNum > MAX_SEQ_NUM) {
    return;
  }

  std::uint8_t flags = buffer[7];

  SetSequenceNumber(seqNum);
  SetAcknowledgmentNumber(ackNum);

  if (flags & packet::flags::ACK) {
    SetAck();
  }

  if (flags & packet::flags::FIN) {
    SetFin();
  }

  if (flags & packet::flags::SYN) {
    SetSyn();
  }

  SetLength(n - HEADER_SIZE);

  if (int len = Length(); len) {
    std::memcpy(static_cast<void*>(Payload()), buffer + HEADER_SIZE, len);
  }
}

std::unique_ptr<packet::buffer_t> packet::makeBuffer() noexcept {
  std::unique_ptr<packet::buffer_t> ptr(new packet::buffer_t[1]);
  return ptr;
}
