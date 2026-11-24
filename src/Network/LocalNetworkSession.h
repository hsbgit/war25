#pragma once

#include <cstdint>
#include <memory>

#include "CommandTransport.h"

namespace Network {

// Hosts an authoritative TCP relay and connects the local human participant
// through the same socket path used by future remote participants.
class LocalNetworkSession final : public ICommandTransport {
public:
	explicit LocalNetworkSession(std::uint16_t port = 24825);
	~LocalNetworkSession() override;

	LocalNetworkSession(const LocalNetworkSession&) = delete;
	LocalNetworkSession& operator=(const LocalNetworkSession&) = delete;

	std::uint16_t localParticipant() const noexcept override { return 0; }
	std::set<std::uint16_t> participants() const override { return {0}; }
	void submitFrame(std::uint64_t tick, const std::vector<Event>& events) override;
	std::vector<CommandFrame> pollFrames() override;
	void resetGame() override;
	std::uint16_t port() const noexcept;

private:
	class Impl;
	std::unique_ptr<Impl> m_impl;
};

} // namespace Network
