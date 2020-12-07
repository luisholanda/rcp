#include "gtest/gtest.h"
#include "rcp.hh"

TEST(PacketTests, packetMakeIsEmpty) {
  auto packet = rcp::packet::make();

  EXPECT_EQ(packet->AcknowledgmentNumber(), 0);
  EXPECT_EQ(packet->SequenceNumber(), 0);
  EXPECT_EQ(packet->Length(), 0);
  EXPECT_FALSE(packet->IsAck());
  EXPECT_FALSE(packet->IsSyn());
  EXPECT_FALSE(packet->IsFin());
}

TEST(PacketTests, packetFlags) {
  auto packet = rcp::packet::make();

  packet->SetAck();
  EXPECT_TRUE(packet->IsAck());
  packet->SetNoAck();
  EXPECT_FALSE(packet->IsAck());

  packet->SetSyn();
  EXPECT_TRUE(packet->IsSyn());
  packet->SetNoSyn();
  EXPECT_FALSE(packet->IsSyn());

  packet->SetFin();
  EXPECT_TRUE(packet->IsFin());
  packet->SetNoFin();
  EXPECT_FALSE(packet->IsFin());
}

TEST(PacketTests, packetNumbers) {
  auto packet = rcp::packet::make();

  packet->SetAcknowledgmentNumber(10);
  EXPECT_EQ(packet->AcknowledgmentNumber(), 10);
  EXPECT_ANY_THROW(packet->SetAcknowledgmentNumber(rcp::packet::MAX_SEQ_NUM + 1));

  packet->SetSequenceNumber(10);
  EXPECT_EQ(packet->SequenceNumber(), 10);
  EXPECT_ANY_THROW(packet->SetSequenceNumber(rcp::packet::MAX_SEQ_NUM + 1));
}

TEST(PacketTests, packetPayload) {
  auto packet = rcp::packet::make();

  packet->SetLength(10);
  EXPECT_EQ(packet->Length(), 10);

  EXPECT_ANY_THROW(packet->SetLength(rcp::packet::MAX_PKT_PAYLOAD_SIZE + 1));
}

TEST(PacketTests, packetFromBuffer) {
  auto buffer = rcp::packet::makeBuffer();

  (*buffer)[0] = 0xF;
  (*buffer)[1] = 0;
  (*buffer)[2] = 0;
  (*buffer)[3] = 0;
  (*buffer)[8] = 0xFF;
  (*buffer)[9] = 0xFF;

  std::uint16_t correctSeqNum;
  std::memcpy(&correctSeqNum, *buffer, 2);

  auto packet = rcp::packet::fromBuffer(std::move(buffer), 10);
  EXPECT_TRUE(packet != nullptr);

  EXPECT_EQ(packet->Length(), 2);
  EXPECT_EQ(packet->SequenceNumber(), correctSeqNum);
  EXPECT_EQ(packet->AcknowledgmentNumber(), 0);
  EXPECT_EQ(packet->Payload()[0], 0xFF);
  EXPECT_EQ(packet->Payload()[1], 0xFF);

  buffer = rcp::packet::makeBuffer();

  (*buffer)[0] = 0xF;
  (*buffer)[1] = 0;
  (*buffer)[2] = 0;
  (*buffer)[3] = 0;
  (*buffer)[8] = 0xFF;
  (*buffer)[9] = 0xFF;

  packet->SetLength(0);
  packet->SetSequenceNumber(0);
  packet->SetAcknowledgmentNumber(0);

  packet->FromBuffer(*buffer, 10);

  EXPECT_EQ(packet->Length(), 2);
  EXPECT_EQ(packet->SequenceNumber(), correctSeqNum);
  EXPECT_EQ(packet->AcknowledgmentNumber(), 0);
}
