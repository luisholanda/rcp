#pragma once

#include <cstdint>
#include <memory>
#include <vector>

namespace rcp {
/// Type used to represent bytes in the RCP API.
using byte = std::uint8_t;

class socket;

/**
 * A RCP packet.
 *
 * @see `packet::make` for info in how to create an instance of this class.
 *
 * The packet is made of two parts: a header, and a payload.
 *
 * ## Header
 *
 * The header has a fixed size of 64bits and have the following structure:
 *
 * ```
 *  0                   1                   2                   3
 *  0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |        Sequence Number        |     Acknowledgment Number     |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |            Not Used           |          Not Used       |A|S|F|
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * ```
 *
 * The first 16 bits represent the sequence number (in the TCP sense)
 * of the first byte in the payload (as in TCP). If `SYN` is set, this
 * field represents the Initial Sequence Number (ISN) and the first
 * byte has sequence number `ISN + 1`. Implementations are free to
 * choose what value of ISN should be used.
 *
 * The next 16 bits represent the acnowledgment number (in the TCP sense),
 * which is the next sequence number that the sender expects to receive.
 *
 * The next 29 aren't used and are reserved for future use. Currently
 * they're set to 0, they MAY be changed.
 *
 * The next 3 bits represent the ACK, SYN and FIN TCP flags.
 *
 * ## Payload
 *
 * Each RCP packet can carry _at most_ 1024 bytes of information (making
 * the maximum packet size 1032). Users are responsible for correctly
 * set the packet payload, @see `packet::Payload` and `packet::SetLength`.
 */
class packet {
public:
  /// Maximum packet payload size.
  static constexpr std::size_t MAX_PKT_PAYLOAD_SIZE = 1024;
  /// Size of a RCP packet.
  static constexpr std::size_t HEADER_SIZE = 4 * sizeof(std::uint16_t);
  /// Maximum packet size.
  static constexpr std::size_t PACKET_SIZE = HEADER_SIZE + MAX_PKT_PAYLOAD_SIZE;
  /// Maximum sequence number value.
  static constexpr std::size_t MAX_SEQ_NUM = 30720;

  /// A buffer with the correct size to hold one packet.
  using buffer_t = byte[PACKET_SIZE];

  /// The sequence number of this packet.
  [[nodiscard]]
  inline constexpr std::uint16_t SequenceNumber() const noexcept {
    return mSeqNum;
  }

  /// The acknowledgment number of this packet.
  [[nodiscard]]
  inline constexpr std::uint16_t AcknowledgmentNumber() const noexcept {
    return mAckNum;
  }

  /// Is the `FIN` flag set?
  [[nodiscard]]
  inline constexpr bool IsFin() const noexcept {
    return (mFlags & flags::FIN) != 0;
  }

  /// Is the `SYN` flag set?
  [[nodiscard]]
  inline constexpr bool IsSyn() const noexcept {
    return (mFlags & flags::SYN) != 0;
  }

  /// Is the `ACK` flag set?
  [[nodiscard]]
  inline constexpr bool IsAck() const noexcept {
    return (mFlags & flags::ACK) != 0;
  }

  /// The length of the packet's payload.
  [[nodiscard]]
  inline constexpr std::uint16_t Length() const noexcept {
    return mLength;
  }

  /**
   * A pointer to the packet's payload.
   *
   * If more data is written to the memory pointed by this pointer,
   * `packet::SetLength` should be called to update the packet length,
   * otherwise, the new data will NOT be sent.
   */
  [[nodiscard]]
  inline byte* Payload() noexcept {
    return mPayload;
  }

  /**
   * Set the sequence number of this packet.
   *
   * @throws if the parameter is bigger than `MAX_SEQ_NUM`.
   */
  void SetSequenceNumber(std::uint16_t seqNum);

  /**
   * Set the acknowledgment number of this packet.
   *
   * @throws if the parameter is bigger than `MAX_SEQ_NUM`.
   */
  void SetAcknowledgmentNumber(std::uint16_t ackNum);

  /// Set the `FIN` flag.
  inline void SetFin() noexcept {
    mFlags |= flags::FIN;
  }

  /// Set the `SYN` flag.
  inline void SetSyn() noexcept {
    mFlags |= flags::SYN;
  }

  /// Set the `ACK` flag.
  inline void SetAck() noexcept {
    mFlags |= flags::ACK;
  }

  /// Unset the `FIN` flag.
  inline void SetNoFin() noexcept {
    mFlags &= ~flags::FIN;
  }

  /// Unset the `SYN` flag.
  inline void SetNoSyn() noexcept {
    mFlags &= ~flags::SYN;
  }

  /// Unset the `ACK` flag.
  inline void SetNoAck() noexcept {
    mFlags &= ~flags::ACK;
  }

  /**
   * Update the packet's payload length.
   *
   * This should be called each time the payload is written to.
   *
   * @paran length is the payload's length.
   */
  void SetLength(std::uint16_t length);

  /// Clears this packet.
  void Clear() noexcept;

  /**
   * Construct an empty packet.
   *
   * Is guaranteed that:
   *
   * * The payload will be empty.
   * * No flags will be set.
   * * Both sequence and acknowledgment numbers will be 0.
   *
   * @returns a new constructed packet.
   */
  static std::unique_ptr<packet> make() noexcept;

  /**
   * Parse the received buffer as a RCP packet.
   *
   * Only allocates if the buffer is valid.
   *
   * @param buffer is the buffer containing the packet bytes.
   * @param n is the valid buffer length.
   * @returns the RCP packet if the contents of the buffer is valid.
   */
  static std::unique_ptr<packet>
  fromBuffer(std::unique_ptr<buffer_t> buffer, std::size_t n) noexcept;

  /// Make a buffer big enough to hold ONE RCP packet.
  static std::unique_ptr<buffer_t> makeBuffer() noexcept;

  /**
   * Parse the received buffer into this RCP packet.
   *
   * The packet isn't changed if the buffer is invalid. Can be used
   * to reduce the number of required allocations in a socket.
   *
   * @param buffer is the buffer containing the packet bytes.
   * @param n is the valid buffer length.
   */
  void FromBuffer(const buffer_t& buffer, std::size_t n) noexcept;
protected:
  struct flags {
    static constexpr unsigned int FIN = 1 << 0;
    static constexpr unsigned int SYN = 1 << 1;
    static constexpr unsigned int ACK = 1 << 2;
  };

  [[nodiscard]]
  inline const void* begin() const noexcept {
    return static_cast<const void*>(this);
  }

  [[nodiscard]]
  inline const void* end() const noexcept {
    return static_cast<const void*>(static_cast<const int*>(begin()) + HEADER_SIZE + mLength);
  }

  inline void PrepareToSend() noexcept {
    mLength = 0;
  }

  friend class socket;
private:
  std::uint16_t mSeqNum;
  std::uint16_t mAckNum;
  std::uint16_t __window = 0;
  unsigned int mLength :10;
  unsigned int __padd :3;
  unsigned int mFlags :3;

  byte mPayload[MAX_PKT_PAYLOAD_SIZE];
};
} // end namespace rcp
