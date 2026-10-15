#include "Network/Socket.hpp"
#include "Platform.hpp"

#if defined(VELO_PLATFORM_WINDOWS)

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#pragma comment(lib, "WS2_32.lib")

struct WinSocketRuntime {
	WinSocketRuntime() {
		WSADATA data{};
		WSAStartup(MAKEWORD(2, 2), &data);
	}
	~WinSocketRuntime() {
		WSACleanup();
	}
}__winsockrt__;

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
	Socket::Status velo::net::Socket::getErrorStatus() {
#if defined(VELO_PLATFORM_WINDOWS)
		switch (WSAGetLastError()) {
		case WSAEWOULDBLOCK:
		case WSAEALREADY:
			return NotReady;
		case WSAECONNABORTED:
		case WSAECONNRESET:
		case WSAETIMEDOUT:
		case WSAENETRESET:
		case WSAENOTCONN:
			return Disconnected;
		case WSAEISCONN:
			return Done;
		default:
			return Error;
		}
#else
		switch (errno) { 
		case EWOULDBLOCK: 
#if EAGAIN != EWOULDBLOCK 
		case EAGAIN: 
#endif 
		case EINPROGRESS: 
		case EALREADY: 
		return NotReady; 
		case ECONNABORTED: 
		case ECONNRESET: 
		case ETIMEDOUT: 
		case ENETRESET: 
		case ENOTCONN: 
		case EPIPE: 
		return Disconnected; 
		case EISCONN: 
		return Done; 
		default: 
			return Error; 
		}
#endif
	}
	Socket::Socket() : qwSocket(Socket::Invalid), bIsBlocking(true) {}

	Socket::~Socket() { close(); }

	Socket::Socket(Socket&& other) noexcept {
		Socket::qwSocket = other.qwSocket;
		Socket::bIsBlocking = other.bIsBlocking;
		other.qwSocket = Socket::Invalid;
		other.bIsBlocking = false;
		
	}

	Socket& Socket::operator=(Socket&& other) noexcept {
		if (this != &other) {
			close();
			Socket::qwSocket = other.qwSocket;
			Socket::bIsBlocking = other.bIsBlocking;
			other.qwSocket = Socket::Invalid;
			other.bIsBlocking = false;
		}
		return *this;
	}

	void Socket::setBlocking(bool value) {
		if (Socket::qwSocket != Socket::Invalid) {
#if defined(VELO_PLATFORM_WINDOWS)
			u_long mode = value ? 0UL : 1UL;

			if (ioctlsocket(Socket::qwSocket, FIONBIO, &mode) != -1) {
				Socket::bIsBlocking = value;
			}
#else 
			const int flags = fcntl(Socket::qwSocket, F_GETFL, 0);

			if (flags == -1) {
				return;
			}
			const int newFlags = value ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);

			if (fcntl(Socket::qwSocket, F_SETFL, newFlags) != -1) {
				Socket::bIsBlocking = value;
			}
#endif
		}
	}

	bool Socket::isBlocking() const {
		return Socket::bIsBlocking;
	}


	void Socket::close() {
		if (Socket::qwSocket != Socket::Invalid) {
#if defined(VELO_PLATFORM_WINDOWS)
			closesocket(static_cast<SOCKET>(Socket::qwSocket));
#else
			::close(static_cast<int>(Socket::qwSocket));
#endif
			Socket::qwSocket = Socket::Invalid;
		}
	}

	void Socket::create() {
		if (Socket::qwSocket == Socket::Invalid) {
			const Qword handle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if (handle == Socket::Invalid) {
				return;
			}
			create(handle);
		}
	}

	Qword Socket::getNativeHandle() const {
		return Socket::qwSocket;
	}

	bool Socket::wait(i32 duration) {
		fd_set readSet;
		FD_ZERO(&readSet);
		FD_SET(qwSocket, &readSet);

		timeval timeout{};
		timeout.tv_sec = duration / 1000;
		timeout.tv_usec = (duration % 1000) * 1000;

		const int result = ::select(
#if defined(VELO_PLATFORM_UNIX)
			static_cast<int>(qwSocket) + 1,
#else
			0,
#endif
			&readSet,
			nullptr,
			nullptr,
			&timeout
		);

		return result > 0 && FD_ISSET(qwSocket, &readSet);
	}

	void Socket::create(Qword handle) {
		if (Socket::qwSocket == Socket::Invalid) {
			Socket::qwSocket = handle;
			setBlocking(isBlocking());

			Dword yes = 1;
			if (setsockopt(Socket::qwSocket, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char*>(&yes), sizeof(yes)) == -1) {
				// failed to set socket option
				return;
			}
		}
	}

}

