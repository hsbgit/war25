#include <gtest/gtest.h>

#include "EventHandling/EventBroker.h"
#include "EventHandling/EventProcessor.h"
#include "Global/PseudoRandom.h"
#include "Network/CommandTransport.h"

#include <vector>

namespace {
class RecordingProcessor final : public EventProcessor {
public:
    void handleEvent(const Event& event) override {
        received.push_back(event.getEventID());
    }

    std::vector<EventID> received;
};

class RecordingTransport final : public Network::ICommandTransport {
public:
	std::uint16_t localParticipant() const noexcept override { return 7; }
	std::set<std::uint16_t> participants() const override { return {7}; }
	void submitFrame(std::uint64_t tick, const std::vector<Event>& events) override {
		submitted.push_back({7, tick, events});
	}
	std::vector<Network::CommandFrame> pollFrames() override {
		std::vector<Network::CommandFrame> result;
		result.swap(incoming);
		return result;
	}
	void resetGame() override { incoming.clear(); submitted.clear(); }

	std::vector<Network::CommandFrame> submitted;
	std::vector<Network::CommandFrame> incoming;
};
}

TEST(Lockstep, WaitsForEveryParticipantAndUsesStableOrder) {
    g_pEventBroker.reset();
    RecordingProcessor processor;
    g_pEventBroker.configureParticipants({1, 2}, 1);

    const auto tick = g_pEventBroker.nextTick();
    g_pEventBroker.addEvent(2, tick, Event(processor.getID(), EventID{22}));
    g_pEventBroker.addEvent(1, tick, Event(processor.getID(), EventID{11}));

    g_pEventBroker.completeFrame(1, tick);
    EXPECT_FALSE(g_pEventBroker.advanceTick());
    EXPECT_TRUE(processor.received.empty());

    g_pEventBroker.completeFrame(2, tick);
    ASSERT_TRUE(g_pEventBroker.advanceTick());
    EXPECT_EQ(processor.received, (std::vector<EventID>{11, 22}));
    EXPECT_EQ(g_pEventBroker.currentTick(), 1u);
}

TEST(Lockstep, EmptyFramesAdvanceAndCommandsNeverRunEarly) {
    g_pEventBroker.reset();
    RecordingProcessor processor;

    g_pEventBroker.addEvent(Event(processor.getID(), EventID{7}));
    EXPECT_TRUE(processor.received.empty());

    g_pEventBroker.completeLocalFrame();
    ASSERT_TRUE(g_pEventBroker.advanceTick());
    EXPECT_EQ(processor.received, (std::vector<EventID>{7}));

    g_pEventBroker.completeLocalFrame();
    EXPECT_TRUE(g_pEventBroker.advanceTick());
    EXPECT_EQ(g_pEventBroker.currentTick(), 2u);
}

TEST(Lockstep, CosmeticRandomnessDoesNotAffectSimulationStream) {
    PseudoRandom::seed(1234);
    const int first = PseudoRandom::rnd(0, 100000);
    const int second = PseudoRandom::rnd(0, 100000);

    PseudoRandom::seed(1234);
    EXPECT_EQ(PseudoRandom::rnd(0, 100000), first);
    (void)PseudoRandom::cosmeticRnd(0, 100000);
    EXPECT_EQ(PseudoRandom::rnd(0, 100000), second);
}

TEST(Lockstep, NetworkTransportMustEchoLocalFrameBeforeItExecutes) {
	g_pEventBroker.reset();
	RecordingProcessor processor;
	RecordingTransport transport;
	g_pEventBroker.attachCommandTransport(&transport);
	g_pEventBroker.addEvent(Event(processor.getID(), EventID{31}, 4, 5));
	g_pEventBroker.completeLocalFrame();
	ASSERT_EQ(transport.submitted.size(), 1u);
	EXPECT_TRUE(processor.received.empty());
	EXPECT_FALSE(g_pEventBroker.advanceTick());

	transport.incoming.push_back(transport.submitted.front());
	ASSERT_TRUE(g_pEventBroker.advanceTick());
	EXPECT_EQ(processor.received, (std::vector<EventID>{31}));
	g_pEventBroker.reset();
}
