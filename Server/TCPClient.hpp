#pragma once
#include "Socket.hpp"
#include <string>

namespace velo {
	class Packet;
	class TCPClient : public Socket{
	public:
		using Socket::Socket;

		TCPClient(TCPClient&&) noexcept = default;
		TCPClient& operator=(TCPClient&&) noexcept = default;

		Word getLocalPort() const;
		const std::string getRemoteAddress() const;
		Word getRemotePort() const;

		void disconnect();

		Status send(const void* data, Qword size, Qword& sent);
		Status send(const Packet& p);
		Status receive(void* data, Qword sizem, Qword& received);

		Status receive(Packet& p);
	private:
	};
}