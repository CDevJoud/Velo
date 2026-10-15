#pragma once
#include "TCPClient.hpp"

namespace velo::net {
	class TCPServer : public Socket, public core::IntrusiveCounted {
	public:
		TCPServer() = default;

		Status listen(u16 port = 8080, const char* ipAddress = "127.0.0.1");
		void stop();

		Status accept(TCPClient& client);
	};
}