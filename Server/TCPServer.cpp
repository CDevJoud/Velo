#include "TCPServer.hpp"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>

#pragma warning(disable:4996)

namespace velo {
	Socket::Status TCPServer::listen(Word port, const std::string& ipAddress) {
		Socket::close();

		Socket::create();

		struct sockaddr_in addr {};
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		addr.sin_addr.s_addr = inet_addr(ipAddress.c_str());
		if (bind(Socket::getNativeHandle(), (sockaddr*)&addr, sizeof(addr)) == -1) {
			return Error;
		}
		if (::listen(Socket::getNativeHandle(), SOMAXCONN) == -1) {
			return Error;
		}

		return Done;
	}
	void TCPServer::stop() {
		Socket::close();
	}
	Socket::Status TCPServer::accept(TCPClient& client) {
		if (Socket::getNativeHandle() == Socket::Invalid) {
			return Error;
		}
		struct sockaddr_in addr{};
		Int32 len = sizeof(addr);
		const Qword remote = ::accept(Socket::getNativeHandle(), (sockaddr*)&addr, (int*)&len);
		if (remote == Socket::Invalid) {
			return Socket::getErrorStatus();
		}
		client.close();
		client.create(remote);

		return Done;
	}
}
