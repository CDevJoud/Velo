#pragma once
#include "TCPClient.hpp"
#include <string>

namespace velo {
	class TCPServer : public Socket {
	public:
		TCPServer() = default;

		Status listen(Word port = 25565, const std::string& ipAddress = "127.0.0.1");
		void stop();
		Status accept(TCPClient& client);
	};
}