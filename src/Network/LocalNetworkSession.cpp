#include "LocalNetworkSession.h"

#include "NetworkProtocol.h"

#include <array>
#include <cerrno>
#include <cstring>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
using SocketHandle = SOCKET;
constexpr SocketHandle InvalidSocket = INVALID_SOCKET;
#else
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
using SocketHandle = int;
constexpr SocketHandle InvalidSocket = -1;
#endif

namespace {

int lastSocketError() {
#ifdef _WIN32
	return WSAGetLastError();
#else
	return errno;
#endif
}

bool wouldBlock(int error) {
#ifdef _WIN32
	return error == WSAEWOULDBLOCK;
#else
	return error == EAGAIN || error == EWOULDBLOCK;
#endif
}

void closeSocket(SocketHandle socket) {
	if (socket == InvalidSocket) return;
#ifdef _WIN32
	closesocket(socket);
#else
	close(socket);
#endif
}

void setNonBlocking(SocketHandle socket) {
#ifdef _WIN32
	u_long enabled = 1;
	if (ioctlsocket(socket, FIONBIO, &enabled) != 0)
		throw std::runtime_error("Could not make network socket non-blocking");
#else
	const int flags = fcntl(socket, F_GETFL, 0);
	if (flags < 0 || fcntl(socket, F_SETFL, flags | O_NONBLOCK) < 0)
		throw std::runtime_error("Could not make network socket non-blocking");
#endif
}

void flush(SocketHandle socket, std::vector<std::uint8_t>& outgoing) {
	std::size_t sentTotal = 0;
	while (sentTotal < outgoing.size()) {
#ifdef _WIN32
		const int sent = send(socket,
			reinterpret_cast<const char*>(outgoing.data() + sentTotal),
			static_cast<int>(outgoing.size() - sentTotal), 0);
#else
		const auto sent = send(socket, outgoing.data() + sentTotal,
			outgoing.size() - sentTotal,
#ifdef MSG_NOSIGNAL
			MSG_NOSIGNAL
#else
			0
#endif
		);
#endif
		if (sent > 0) {
			sentTotal += static_cast<std::size_t>(sent);
			continue;
		}
		const int error = lastSocketError();
		if (wouldBlock(error)) break;
		throw std::runtime_error("Network connection failed while sending");
	}
	if (sentTotal)
		outgoing.erase(outgoing.begin(), outgoing.begin() +
			static_cast<std::ptrdiff_t>(sentTotal));
}

void receiveAvailable(SocketHandle socket, std::vector<std::uint8_t>& incoming) {
	std::array<std::uint8_t, 4096> buffer{};
	while (true) {
#ifdef _WIN32
		const int received = recv(socket, reinterpret_cast<char*>(buffer.data()),
			static_cast<int>(buffer.size()), 0);
#else
		const auto received = recv(socket, buffer.data(), buffer.size(), 0);
#endif
		if (received > 0) {
			incoming.insert(incoming.end(), buffer.begin(),
				buffer.begin() + received);
			continue;
		}
		if (received == 0)
			throw std::runtime_error("Network connection was closed");
		const int error = lastSocketError();
		if (wouldBlock(error)) return;
		throw std::runtime_error("Network connection failed while receiving");
	}
}

template<typename Consumer>
void consumeMessages(std::vector<std::uint8_t>& incoming, Consumer consume) {
	while (!incoming.empty()) {
		const auto size = Network::Protocol::messageSize(incoming);
		if (!size) return;
		if (*size == 0)
			throw std::runtime_error("Received an invalid network message header");
		const auto payload = std::span<const std::uint8_t>(incoming).subspan(
			Network::Protocol::HeaderSize,
			*size - Network::Protocol::HeaderSize);
		const auto frame = Network::Protocol::decodePayload(payload);
		if (!frame)
			throw std::runtime_error("Received an invalid command frame");
		consume(*frame, std::span<const std::uint8_t>(incoming).first(*size));
		incoming.erase(incoming.begin(), incoming.begin() +
			static_cast<std::ptrdiff_t>(*size));
	}
}

} // namespace

namespace Network {

class LocalNetworkSession::Impl {
public:
	explicit Impl(std::uint16_t requestedPort) {
#ifdef _WIN32
		WSADATA data{};
		if (WSAStartup(MAKEWORD(2, 2), &data) != 0)
			throw std::runtime_error("Could not initialize Winsock");
		m_winsockStarted = true;
#endif
		try {
			start(requestedPort);
		} catch (...) {
			shutdown();
			throw;
		}
	}

	~Impl() { shutdown(); }

	void start(std::uint16_t requestedPort) {
		m_listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_listener == InvalidSocket)
			throw std::runtime_error("Could not create lockstep server socket");
		int reuse = 1;
#ifdef _WIN32
		setsockopt(m_listener, SOL_SOCKET, SO_REUSEADDR,
			reinterpret_cast<const char*>(&reuse), sizeof(reuse));
#else
		setsockopt(m_listener, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
#endif
		sockaddr_in serverAddress{};
		serverAddress.sin_family = AF_INET;
		serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
		serverAddress.sin_port = htons(requestedPort);
		if (bind(m_listener, reinterpret_cast<sockaddr*>(&serverAddress),
			sizeof(serverAddress)) != 0)
			throw std::runtime_error("Could not bind lockstep server port " +
				std::to_string(requestedPort));
		if (listen(m_listener, 1) != 0)
			throw std::runtime_error("Could not listen on lockstep server socket");

		sockaddr_in bound{};
#ifdef _WIN32
		int boundLength = sizeof(bound);
#else
		socklen_t boundLength = sizeof(bound);
#endif
		if (getsockname(m_listener, reinterpret_cast<sockaddr*>(&bound),
			&boundLength) != 0)
			throw std::runtime_error("Could not query lockstep server port");
		m_port = ntohs(bound.sin_port);

		m_client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (m_client == InvalidSocket)
			throw std::runtime_error("Could not create local lockstep client socket");
		sockaddr_in loopback{};
		loopback.sin_family = AF_INET;
		loopback.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
		loopback.sin_port = htons(m_port);
		if (connect(m_client, reinterpret_cast<sockaddr*>(&loopback),
			sizeof(loopback)) != 0)
			throw std::runtime_error("Could not connect local player to lockstep server");
		m_serverPeer = accept(m_listener, nullptr, nullptr);
		if (m_serverPeer == InvalidSocket)
			throw std::runtime_error("Lockstep server could not accept local player");
		closeSocket(m_listener);
		m_listener = InvalidSocket;
#ifdef SO_NOSIGPIPE
		int noSignal = 1;
		setsockopt(m_client, SOL_SOCKET, SO_NOSIGPIPE, &noSignal, sizeof(noSignal));
		setsockopt(m_serverPeer, SOL_SOCKET, SO_NOSIGPIPE, &noSignal, sizeof(noSignal));
#endif
		setNonBlocking(m_client);
		setNonBlocking(m_serverPeer);
	}

	void shutdown() noexcept {
		closeSocket(m_serverPeer);
		closeSocket(m_client);
		closeSocket(m_listener);
		m_serverPeer = m_client = m_listener = InvalidSocket;
#ifdef _WIN32
		if (m_winsockStarted) WSACleanup();
		m_winsockStarted = false;
#endif
	}

	void pump() {
		// The local client and authoritative relay are intentionally separate
		// TCP endpoints: commands become visible only after the server echo.
		flush(m_client, m_clientOutgoing);
		receiveAvailable(m_serverPeer, m_serverIncoming);
		consumeMessages(m_serverIncoming,
			[this](const CommandFrame&, std::span<const std::uint8_t> message) {
				m_serverOutgoing.insert(m_serverOutgoing.end(),
					message.begin(), message.end());
			});
		flush(m_serverPeer, m_serverOutgoing);
		receiveAvailable(m_client, m_clientIncoming);
		consumeMessages(m_clientIncoming,
			[this](const CommandFrame& frame, std::span<const std::uint8_t>) {
				m_received.push_back(frame);
			});
	}

	void resetGame() {
		m_clientOutgoing.clear();
		m_serverIncoming.clear();
		m_serverOutgoing.clear();
		m_clientIncoming.clear();
		m_received.clear();
		std::vector<std::uint8_t> discarded;
		receiveAvailable(m_serverPeer, discarded);
		discarded.clear();
		receiveAvailable(m_client, discarded);
	}

	std::uint16_t m_port = 0;
	SocketHandle m_listener = InvalidSocket;
	SocketHandle m_client = InvalidSocket;
	SocketHandle m_serverPeer = InvalidSocket;
	std::vector<std::uint8_t> m_clientOutgoing;
	std::vector<std::uint8_t> m_serverIncoming;
	std::vector<std::uint8_t> m_serverOutgoing;
	std::vector<std::uint8_t> m_clientIncoming;
	std::vector<CommandFrame> m_received;
#ifdef _WIN32
	bool m_winsockStarted = false;
#endif
};

LocalNetworkSession::LocalNetworkSession(std::uint16_t port)
	: m_impl(std::make_unique<Impl>(port)) {}

LocalNetworkSession::~LocalNetworkSession() = default;

void LocalNetworkSession::submitFrame(std::uint64_t tick,
	const std::vector<Event>& events) {
	const auto message = Protocol::encodeMessage({localParticipant(), tick, events});
	m_impl->m_clientOutgoing.insert(m_impl->m_clientOutgoing.end(),
		message.begin(), message.end());
}

std::vector<CommandFrame> LocalNetworkSession::pollFrames() {
	m_impl->pump();
	std::vector<CommandFrame> frames;
	frames.swap(m_impl->m_received);
	return frames;
}

void LocalNetworkSession::resetGame() {
	m_impl->resetGame();
}

std::uint16_t LocalNetworkSession::port() const noexcept {
	return m_impl->m_port;
}

} // namespace Network
