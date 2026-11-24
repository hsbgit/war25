#pragma once

#include <cstdint>
#include <set>
#include <vector>

#include "EventHandling/Event.h"

namespace Network {

struct CommandFrame {
	std::uint16_t participant = 0;
	std::uint64_t tick = 0;
	std::vector<Event> events;
};

class ICommandTransport {
public:
	virtual ~ICommandTransport() = default;
	virtual std::uint16_t localParticipant() const noexcept = 0;
	virtual std::set<std::uint16_t> participants() const = 0;
	virtual void submitFrame(std::uint64_t tick, const std::vector<Event>& events) = 0;
	virtual std::vector<CommandFrame> pollFrames() = 0;
	virtual void resetGame() = 0;
};

} // namespace Network
