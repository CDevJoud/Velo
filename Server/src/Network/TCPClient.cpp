#include "Network/TCPClient.hpp"
#include "Platform.hpp"

#if defined(VELO_PLATFORM_WINDOWS)

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

#include <EASTL/fixed_vector.h>

#include <sstream>
namespace velo::net {
	TCPClient::TCPClient(const std::reference_wrapper<core::QEventBus>& qLogBus) : qLogBus(qLogBus) {}

	u16 TCPClient::getLocalPort() const {
		if (getNativeHandle() != Socket::Invalid) {
			struct sockaddr_in address {};
			platform::socklen_type size = sizeof(address);
			if (getsockname(getNativeHandle(), (sockaddr*)&address, &size) != -1) {
				return ntohs(address.sin_port);
			}
		}
		return u16(~0);
	}

	const eastl::string TCPClient::getRemoteAddress() const {
		if (getNativeHandle() != Socket::Invalid) {
			sockaddr_in address{};
			platform::socklen_type size = sizeof(address);

			if (getpeername(getNativeHandle(), reinterpret_cast<sockaddr*>(&address), &size) == 0) {
				char ipBuffer[INET_ADDRSTRLEN];

				if (inet_ntop(AF_INET, &address.sin_addr, ipBuffer, sizeof(ipBuffer))) {
					return ipBuffer;
				}
			}
		}
		return eastl::string();
	}

	u16 TCPClient::getRemotePort() const {
		if (getNativeHandle() != Socket::Invalid) {
			sockaddr_in address{};
			platform::socklen_type size = sizeof(address);
			if (getpeername(getNativeHandle(), reinterpret_cast<sockaddr*>(&address), &size) != -1) {
				return ntohs(address.sin_port);
			}
		}

		return u16(~0);
	}

	void TCPClient::disconnect() {
		close();
	}

	Socket::Status TCPClient::send(const void* data, u64 size, u64& sent) {
		if (!data || (size == 0)) {
			//LOG_ERROR_TRACE("Empty 'data' or 'size == 0'")
			return Socket::Status::Error;
		}

		i32 result = 0;
		for (sent = 0; sent < size; sent += static_cast<u64>(result)) {
			result = static_cast<i32>(::send(getNativeHandle(), static_cast<const char*>(data), static_cast<i32>(size - sent), 0));

			if (result < 0) {
				Socket::Status status = getErrorStatus();
				if ((status == Socket::NotReady) && sent) {
					return Socket::Partial;
				}

				return status;
			}
		}
		return Socket::Status::Done;
	}

	Socket::Status TCPClient::send(const Http& http) {
		eastl::string res;
		res.append_sprintf("%s %d %s\r\n", http.version.c_str(), http.n16ResponseCode, http.respMsg.c_str());
        for (const auto& [name, value] : http.fields) {
            res.append_sprintf("%s: %s\r\n", name.c_str(), value.c_str());
        }
		res.append("\r\n");

		res.append(http.body);

		Socket::Status status = Socket::Done;
		u64 sent = 0;
		/*u64 amountSent = 0;
		do {
			status = send(res.data() + amountSent, res.size(), sent);
			if (status == Socket::Error) {
				return Socket::Error;
			}
			amountSent += sent;
		} while (sent == amountSent);*/
		status = send(res.data(), res.size(), sent);
		return status;
	}

	Socket::Status TCPClient::receive(void* data, u64 size, u64& received) {
		received = 0;
		if (!data) {
			//LOG_ERROR_TRACE("Invalid data! data is nullptr");
			return Socket::Status::Error;
		}

		if (getNativeHandle() == Socket::Invalid) {
			//LOG_ERROR_TRACE("[TCPClient] my socket is Invalid! you forgot to create me? or used after free?");
			return Socket::Status::Error;
		}

		const i32 sizerecv = static_cast<i32>(recv(getNativeHandle(), static_cast<char*>(data), static_cast<i32>(size), 0));
		received = sizerecv;
		if (sizerecv > 0) {
			return Socket::Status::Done;
		}
		if (sizerecv == 0) {
			return Socket::Status::Disconnected;
		}
		return getErrorStatus();
	}
	Socket::Status TCPClient::receive(Http& http) {
		u64 r = 0;
		u64 amountReceived = 0;
		char buffer[128]{};
		eastl::string bin;
		Socket::Status status = Socket::Done;
		i32 chances = 0;
		setBlocking(false);
		do {

			if (chances == 10) {
				//bad client. disconnect!
				close();
				return Socket::Disconnected;
			}
			status = receive(buffer, 128, r);

			if (status == Socket::NotReady) {
				wait(10);
				chances++;
				continue;
			}

			if (status == Socket::Error) {
				return status;
			}

			bin.append(buffer, 128);
			amountReceived += r;
		} while (r == 128 || r == (~0));
		setBlocking(true);
		eastl_size_t headerEnd = bin.find("\r\n\r\n");

		if (headerEnd == eastl::string::npos) {
			//Invalid Http request: No header body sperator found!
			return Socket::Error;
		}


		std::string method, url, version;
		std::istringstream iss(bin.substr(0, headerEnd).data());
		iss >> method >> url >> version;

		std::string line;
		eastl_size_t contentLength = 0;

		while (std::getline(iss, line, '\n')) {
			if (!line.empty() && line.back() == '\r') line.pop_back();

			std::string fieldName, fieldValue;
			auto strpos = line.find(':');
			if (strpos != std::string::npos) {
				fieldName = line.substr(0, strpos);
				fieldValue = line.substr(strpos + 2);

				http.fields[fieldName.c_str()] = fieldValue.c_str();

				if (fieldName == "Content-Length") {
					contentLength = std::stoi(fieldValue);
				}
			}
		}
		if (method == "POST" && contentLength > 0) {
			http.body = bin.substr(headerEnd + 4, contentLength);
		}

		http.method = method.c_str();
		http.url = url.c_str();
		http.version = version.c_str();
		return status;
	}
}