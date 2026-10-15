#include "Network/TCPServer.hpp"

#include "Platform.hpp"

#if defined(VELO_PLATFORM_WINDOWS)

#pragma warning(disable:4996)

#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#pragma comment(lib, "WS2_32.lib")

#elif defined(VELO_PLATFORM_UNIX)

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#else

#error unsupported networking platform

#endif

namespace velo::net {
	Socket::Status velo::net::TCPServer::listen(u16 port, const char* ipAddress) {
		Socket::close();

		Socket::create();

		struct sockaddr_in addr {};
		addr.sin_family = AF_INET;
		addr.sin_port = htons(port);
		addr.sin_addr.s_addr = inet_addr(ipAddress);
#if defined(VELO_PLATFORM_UNIX)
		const int reuse = 1;
		setsockopt(Socket::getNativeHandle(), SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
#endif
		if (::bind(Socket::getNativeHandle(), (sockaddr*)&addr, sizeof(addr)) == -1) {
			return Error;
		}
		if (::listen(Socket::getNativeHandle(), SOMAXCONN) == -1) {
			return Error;
		}
		return Done;
	}
	void TCPServer::stop() {
#if defined(VELO_PLATFORM_UNIX)
		if (Socket::getNativeHandle() != Socket::Invalid) {
			::shutdown(static_cast<int>(Socket::getNativeHandle()), SHUT_RDWR);
		}
#endif
		Socket::close();
	}
	Socket::Status TCPServer::accept(TCPClient& client) {
		if (Socket::getNativeHandle() == Socket::Invalid) {
			return Error;
		}
		struct sockaddr_in addr {};
		platform::socklen_type len = sizeof(addr);
		const Qword remote = ::accept(Socket::getNativeHandle(), (sockaddr*)&addr, &len);
		if (remote == Socket::Invalid) {
			return Socket::getErrorStatus();
		}
		client.close();
		client.create(remote);

		return Done;
	}
}
