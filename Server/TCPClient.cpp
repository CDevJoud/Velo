#pragma warning(disable: 4996)
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include "TCPClient.hpp"

#include "Packet.hpp"

#define LOG_INFO(msg)  qBus.get().post(event::Log(event::Log::Severity::INFO, msg));
#define LOG_DEBUG(msg) qBus.get().post(event::Log(event::Log::Severity::DEBUG, msg));
#define LOG_WARN(msg)  qBus.get().post(event::Log(event::Log::Severity::WARN, msg));
#define LOG_ERROR(msg) qBus.get().post(event::Log(event::Log::Severity::ERROR, msg));
#define LOG_FATAL(msg) qBus.get().post(event::Log(event::Log::Severity::FATAL, msg));
#define LOG_INFO_TRACE(msg)  qBus.get().post(event::Log(event::Log::Severity::INFO, msg, "default", std::source_location::current()));
#define LOG_DEBUG_TRACE(msg) qBus.get().post(event::Log(event::Log::Severity::DEBUG, msg, "default", std::source_location::current()));
#define LOG_WARN_TRACE(msg)  qBus.get().post(event::Log(event::Log::Severity::WARN, msg, "default", std::source_location::current()));
#define LOG_ERROR_TRACE(msg) qBus.get().post(event::Log(event::Log::Severity::ERROR, msg, "default", std::source_location::current()));
#define LOG_FATAL_TRACE(msg) qBus.get().post(event::Log(event::Log::Severity::FATAL, msg, "default", std::source_location::current()));


namespace velo {
	TCPClient::TCPClient(const std::reference_wrapper<QEventBus>& qBus) : qBus(qBus) {
		
	}
	Word TCPClient::getLocalPort() const {
		if (Socket::getNativeHandle() != Socket::Invalid) {
			struct sockaddr_in address {};
			Int32 size = sizeof(address);
			if (getsockname(Socket::getNativeHandle(), (sockaddr*)&address, &size) != -1) {
				return ntohs(address.sin_port);
			}
		}
		return (Word)(~0);
	}
	const std::string TCPClient::getRemoteAddress() const {
		if (Socket::getNativeHandle() != Socket::Invalid) {
			SOCKADDR_IN address{};
			Int32 size = sizeof(address);

			if (getpeername(Socket::getNativeHandle(), reinterpret_cast<sockaddr*>(&address), &size) == 0) {
				char ipBuffer[INET_ADDRSTRLEN];

				if (inet_ntop(AF_INET, &address.sin_addr, ipBuffer, sizeof(ipBuffer))) {
					return ipBuffer;
				}
			}
		}
	}
	Word TCPClient::getRemotePort() const {
		if (Socket::getNativeHandle() != Socket::Invalid) {
			SOCKADDR_IN address{};
			Int32 size = sizeof(address);
			if (getpeername(Socket::getNativeHandle(), reinterpret_cast<sockaddr*>(&address), &size) != -1) {
				return ntohs(address.sin_port);
			}
		}

		return Word(~0);
	}
	void TCPClient::disconnect() {
		Socket::close();
	}
	Socket::Status TCPClient::send(const void* data, Qword size, Qword& sent) {
		if (!data || (size == 0)) {
			LOG_ERROR_TRACE("Empty 'data' or 'size == 0'")
			return Status::Error;
		}

		Int32 result = 0;
		for (sent = 0; sent < size; sent += static_cast<Qword>(result)) {
			result = static_cast<Int32>(::send(Socket::getNativeHandle(), static_cast<const char*>(data), static_cast<Int32>(size - sent), 0));

			if (result < 0) {
				Status status = Socket::getErrorStatus();
				if ((status == NotReady) && sent) {
					return Partial;
				}

				return status;
			}
		}
		return Status::Done;
	}

	Socket::Status TCPClient::send(const Packet& p) {
		Qword r;

		return TCPClient::send(p.bytes.data(), p.bytes.size(), r);
	}

	Socket::Status TCPClient::receive(void* data, Qword size, Qword& received) {
		received = 0;
		if (!data) {
			LOG_ERROR_TRACE("Invalid data! data is nullptr");
			return Status::Error;
		}

		if (Socket::getNativeHandle() == Socket::Invalid) {
			LOG_ERROR_TRACE("[TCPClient] my socket is Invalid! you forgot to create me? or used after free?");
			return Status::Error;
		}

		const Int32 sizerecv = static_cast<Int32>(recv(Socket::getNativeHandle(), static_cast<char*>(data), static_cast<Int32>(size), 0));
		received = sizerecv;
		if (sizerecv > 0) {
			return Status::Done;
		}
		if (sizerecv == 0) {
			return Status::Disconnected;
		}
		return Socket::getErrorStatus();
	}
    Socket::Status TCPClient::receive(Packet& p) {
		Qword r = 0;
		
		Int32 packsize = 0;
		Packet::ID PID = Packet::ID::Invalid;
		TCPClient::receive(&packsize, sizeof(Int32), r);
		TCPClient::receive(&PID, sizeof(Packet::ID), r);
		p.nPID = PID;
		packsize = std::byteswap(packsize);
		Int32 requestedPacketSize = 0;
		switch (PID) {
		case velo::Packet::ID::DebugOptions:
		{
			p.bytes.resize(packsize);
			p.nPID = Packet::ID::DebugOptions;
			break;
		}
		case velo::Packet::ID::PreLogin:
		{
			p.bytes.resize(packsize);
			requestedPacketSize = PRE_LOGIN_PACKET_SIZE;
			p.nPID = Packet::ID::PreLogin;
			break;
		}
		case velo::Packet::ID::KeepAlive:
		{
			p.bytes.resize(packsize);
			requestedPacketSize = KEEP_ALIVE_PACKET_SIZE;
			p.nPID = Packet::ID::KeepAlive;
			break;
		}
		case velo::Packet::ID::Login:
		{
			p.bytes.resize(packsize);
			requestedPacketSize = LOGIN_PACKET_SIZE;
			p.nPID = Packet::ID::Login;
			break;
		}
		case velo::Packet::ID::Invalid:
			break;
		default:
			LOG_INFO("[TCPClient] Packet ID of " + std::to_string(static_cast<Byte>(PID)) + " is not a valid LCE packet")
			break;
		}

		if (!p.bytes.empty()) {
			Status res = TCPClient::receive(p.bytes.data(), packsize, r);
			if (res != Error && res != Disconnected) {
				return Done;
			}
			return res;
		}
		else {
			return Error;
		}
    }
}
