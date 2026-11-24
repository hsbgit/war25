#pragma once

#include <cstddef>
#include <bit>
#include <cstdint>
#include <limits>
#include <optional>
#include <span>
#include <vector>

#include "CommandTransport.h"

namespace Network::Protocol {

inline constexpr std::uint32_t Magic = 0x5732354eu; // "W25N"
inline constexpr std::size_t HeaderSize = 8;
inline constexpr std::size_t MaximumPayloadSize = 1024 * 1024;
inline constexpr std::uint8_t CommandFrameType = 1;

inline void appendUnsigned(std::vector<std::uint8_t>& bytes,
	std::uint64_t value, std::size_t width) {
	for (std::size_t i = 0; i < width; ++i) {
		const std::size_t shift = (width - i - 1) * 8;
		bytes.push_back(static_cast<std::uint8_t>(value >> shift));
	}
}

inline bool readUnsigned(std::span<const std::uint8_t> bytes,
	std::size_t& offset, std::size_t width, std::uint64_t& value) {
	if (width > sizeof(value) || offset + width > bytes.size()) return false;
	value = 0;
	for (std::size_t i = 0; i < width; ++i)
		value = (value << 8) | bytes[offset++];
	return true;
}

inline std::vector<std::uint8_t> encodePayload(const CommandFrame& frame) {
	std::vector<std::uint8_t> payload;
	payload.reserve(15 + frame.events.size() * 20);
	payload.push_back(CommandFrameType);
	appendUnsigned(payload, frame.participant, 2);
	appendUnsigned(payload, frame.tick, 8);
	appendUnsigned(payload, frame.events.size(), 4);
	for (const Event& event : frame.events) {
		appendUnsigned(payload, event.eventProcessorID(), 8);
		appendUnsigned(payload, event.getEventID(), 2);
		appendUnsigned(payload, std::bit_cast<std::uint32_t>(event.tileX()), 4);
		appendUnsigned(payload, std::bit_cast<std::uint32_t>(event.tileY()), 4);
		appendUnsigned(payload, event.optionalData(), 2);
	}
	return payload;
}

inline std::vector<std::uint8_t> encodeMessage(const CommandFrame& frame) {
	const auto payload = encodePayload(frame);
	std::vector<std::uint8_t> message;
	message.reserve(HeaderSize + payload.size());
	appendUnsigned(message, Magic, 4);
	appendUnsigned(message, payload.size(), 4);
	message.insert(message.end(), payload.begin(), payload.end());
	return message;
}

inline std::optional<CommandFrame> decodePayload(
	std::span<const std::uint8_t> payload) {
	if (payload.empty() || payload.front() != CommandFrameType) return std::nullopt;
	std::size_t offset = 1;
	std::uint64_t participant = 0, tick = 0, count = 0;
	if (!readUnsigned(payload, offset, 2, participant) ||
		!readUnsigned(payload, offset, 8, tick) ||
		!readUnsigned(payload, offset, 4, count) || count > 4096)
		return std::nullopt;
	if (payload.size() - offset != count * 20) return std::nullopt;
	CommandFrame frame{static_cast<std::uint16_t>(participant), tick, {}};
	frame.events.reserve(static_cast<std::size_t>(count));
	for (std::uint64_t i = 0; i < count; ++i) {
		std::uint64_t processor = 0, eventID = 0, tileX = 0, tileY = 0, optional = 0;
		if (!readUnsigned(payload, offset, 8, processor) ||
			!readUnsigned(payload, offset, 2, eventID) ||
			!readUnsigned(payload, offset, 4, tileX) ||
			!readUnsigned(payload, offset, 4, tileY) ||
			!readUnsigned(payload, offset, 2, optional)) return std::nullopt;
		frame.events.emplace_back(processor, static_cast<EventID>(eventID),
			std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(tileX)),
			std::bit_cast<std::int32_t>(static_cast<std::uint32_t>(tileY)),
			static_cast<std::uint16_t>(optional));
	}
	return frame;
}

inline std::optional<std::size_t> messageSize(
	std::span<const std::uint8_t> bytes) {
	if (bytes.size() < HeaderSize) return std::nullopt;
	std::size_t offset = 0;
	std::uint64_t magic = 0, payloadSize = 0;
	if (!readUnsigned(bytes, offset, 4, magic) || magic != Magic ||
		!readUnsigned(bytes, offset, 4, payloadSize) ||
		payloadSize > MaximumPayloadSize)
		return std::size_t{0};
	if (bytes.size() < HeaderSize + payloadSize) return std::nullopt;
	return HeaderSize + static_cast<std::size_t>(payloadSize);
}

} // namespace Network::Protocol
