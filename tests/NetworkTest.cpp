#include <gtest/gtest.h>

#include "Network/LocalNetworkSession.h"
#include "Network/NetworkProtocol.h"

TEST(NetworkProtocol, CommandFrameRoundTripPreservesEveryEventField) {
	const Network::CommandFrame source{3, 0x0102030405060708ULL, {
		Event(0x1122334455667788ULL, EventID{513}, -7, 42, 65530)
	}};
	const auto payload = Network::Protocol::encodePayload(source);
	const auto decoded = Network::Protocol::decodePayload(payload);
	ASSERT_TRUE(decoded.has_value());
	ASSERT_EQ(decoded->events.size(), 1u);
	EXPECT_EQ(decoded->participant, source.participant);
	EXPECT_EQ(decoded->tick, source.tick);
	EXPECT_EQ(decoded->events[0].eventProcessorID(), 0x1122334455667788ULL);
	EXPECT_EQ(decoded->events[0].getEventID(), 513);
	EXPECT_EQ(decoded->events[0].tileX(), -7);
	EXPECT_EQ(decoded->events[0].tileY(), 42);
	EXPECT_EQ(decoded->events[0].optionalData(), 65530);
}

TEST(NetworkProtocol, TruncatedAndOversizedMessagesAreRejected) {
	auto message = Network::Protocol::encodeMessage({0, 1, {}});
	message.pop_back();
	EXPECT_FALSE(Network::Protocol::messageSize(message).has_value());
	std::vector<std::uint8_t> invalid;
	Network::Protocol::appendUnsigned(invalid, Network::Protocol::Magic, 4);
	Network::Protocol::appendUnsigned(invalid,
		Network::Protocol::MaximumPayloadSize + 1, 4);
	EXPECT_EQ(Network::Protocol::messageSize(invalid), 0u);
}

TEST(NetworkSession, LocalHumanFrameTravelsThroughTcpServerEcho) {
	Network::LocalNetworkSession session(0);
	ASSERT_NE(session.port(), 0);
	session.submitFrame(9, {Event(77, EventID{12}, 3, 4, 5)});
	const auto frames = session.pollFrames();
	ASSERT_EQ(frames.size(), 1u);
	EXPECT_EQ(frames[0].participant, 0);
	EXPECT_EQ(frames[0].tick, 9u);
	ASSERT_EQ(frames[0].events.size(), 1u);
	EXPECT_EQ(frames[0].events[0].eventProcessorID(), 77u);
	EXPECT_EQ(frames[0].events[0].tileX(), 3);
}
