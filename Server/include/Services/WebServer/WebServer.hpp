#pragma once
#include "Network/TCPServer.hpp"

namespace velo::services {
	class WebServer : public core::IntrusiveCounted {
	public:
		WebServer(const std::reference_wrapper<core::QEventChannels>& qc);

		~WebServer();
		bool instantiateService();

		bool runService();

		bool isServiceRunning() const;

		void haltService();
	private:
		static void tcpWebServiceProc(const core::Intrusive<WebServer>& self);

		const std::reference_wrapper<core::QEventChannels> qc;
		std::thread tcpServerProc;
		core::Intrusive<net::TCPServer> tcpServer = nullptr;
		bool bIsServiceRunning = false, bIsServiceTerminated = false;

	};
}
