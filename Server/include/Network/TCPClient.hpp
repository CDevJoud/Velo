#pragma once
#include "Core/Intrusive.hpp"
#include "Core/QEventBus.hpp"
#include "Socket.hpp"
#include "Http.hpp"
namespace velo::net {
	class TCPClient : public Socket, public core::IntrusiveCounted {
	public:

		TCPClient(const std::reference_wrapper<core::QEventBus>& qLogBus);

		TCPClient(TCPClient&& client) noexcept = default;
		TCPClient& operator=(TCPClient&&) noexcept = default;

		u16 getLocalPort() const;
		const eastl::string getRemoteAddress() const;
		u16 getRemotePort() const;

		void disconnect();

		Status send(const void* data, u64 size, u64& sent);
		Status send(const Http& http);
		Status receive(void* data, u64 size, u64& received);
		Status receive(Http& http);
	private:
		std::reference_wrapper<core::QEventBus> qLogBus;
	};
}

