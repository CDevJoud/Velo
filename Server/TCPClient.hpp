#pragma once
#include "Socket.hpp"
#include <string>
#include "QEventBus.hpp"
#include "Intrusive.hpp"

namespace velo {
	class Packet;
	class TCPClient : public Socket, public IntrusiveCounted{
	public:
		using Socket::Socket;

		TCPClient(const std::reference_wrapper<QEventBus>& qBus);

		TCPClient(TCPClient&&) noexcept = default;
		TCPClient& operator=(TCPClient&&) noexcept = default;

		Word getLocalPort() const;
		const std::string getRemoteAddress() const;
		Word getRemotePort() const;

		void disconnect() ;

		Status send(const void* data, Qword size, Qword& sent);
		Status send(const Packet& p);
		Status receive(void* data, Qword sizem, Qword& received);

		Status receive(Packet& p);
	private:
		std::reference_wrapper<QEventBus> qBus;
	};

	namespace event::client {
		struct Connect : public IEvent {
			IEVENT_DECLARE_EVENT_NAME("client.connect")
				Connect(
					const Intrusive<TCPClient>& client,
					const std::reference_wrapper<LCEServer>& server
				) : tcpClient(client), server(server) {}
			Intrusive<TCPClient> tcpClient;
			std::reference_wrapper<LCEServer> server;
		};
		struct Disconnect : public IEvent {
			IEVENT_DECLARE_EVENT_NAME("client.disconnect")
				Disconnect(
					const Intrusive<TCPClient>& client,
					const std::reference_wrapper<LCEServer>& server
				) : tcpClient(client), server(server) {}
			Intrusive<TCPClient> tcpClient;
			std::reference_wrapper<LCEServer> server;
		};
	}
}