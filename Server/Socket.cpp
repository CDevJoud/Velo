#include "Socket.hpp"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>

namespace velo {
	Socket::Status Socket::getErrorStatus() {
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
	}
	Socket::Socket() : 
		qwSocket(Socket::Invalid), 
		bIsBlocking(true) {

	}

	Socket::~Socket() {
	
	}

	Socket::Socket(Socket&& other) noexcept {
		Socket::qwSocket = other.qwSocket;
		Socket::bIsBlocking = other.bIsBlocking;
		other.qwSocket = Socket::Invalid;
	}

	Socket& Socket::operator=(Socket&& other) noexcept {
		if (this != &other) {
			Socket::close();
			Socket::qwSocket = other.qwSocket;
			Socket::bIsBlocking = other.bIsBlocking;
			other.qwSocket = Socket::Invalid;
		}
		return *this;
	}

	void Socket::setBlocking(bool value) {
		if (Socket::qwSocket != Socket::Invalid) {
			Dword blocking = value ? 0 : 1;
			ioctlsocket(Socket::qwSocket, static_cast<Int32>(FIONBIO), (u_long*)&blocking);
			Socket::bIsBlocking = value;
		}
	}

	bool Socket::isBlocking() const {
		return Socket::bIsBlocking;
	}

	Qword Socket::getNativeHandle() const {
		return Socket::qwSocket;
	}

	void Socket::close() {
		if (Socket::qwSocket == Socket::Invalid) {
			closesocket(Socket::qwSocket);
			Socket::qwSocket = Socket::Invalid;
		}
	}

	void Socket::create() {
		if (Socket::qwSocket == Socket::Invalid) {
			const Qword handle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if (handle == Socket::Invalid) {
				return; // Failed to create socket!
			}
			Socket::create(handle);
		}
	}

	void Socket::create(Qword handle) {
		if (Socket::qwSocket == Socket::Invalid) {
			Socket::qwSocket = handle;
			Socket::setBlocking(Socket::isBlocking());

			Dword yes = 1;
			if (setsockopt(Socket::qwSocket, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char*>(&yes), sizeof(yes)) == -1) {
				// failed to set socket option
				OutputDebugStringA("Failed To Set Socket Option\n");
			}
		}
	}

}
