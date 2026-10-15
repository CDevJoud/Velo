#include "Services/WebServer/WebServer.hpp"
#include "util.hpp"

//just for testing
#include <Windows.h>

using velo::core::Intrusive;
using velo::core::QEventChannels;
using velo::net::TCPClient;
using velo::net::TCPServer;
using velo::net::Socket;
using velo::net::Http;

#include <string>
namespace velo::services {

	char* loadFile(const char* name) {
		HANDLE hFile = CreateFileA(name, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		if (hFile == INVALID_HANDLE_VALUE) {
			return nullptr;
		}

		size_t size;
		GetFileSizeEx(hFile, (PLARGE_INTEGER)&size);

		char* buffer = (char*)malloc(size);

		DWORD r;
		ReadFile(hFile, buffer, size, &r, 0);
		CloseHandle(hFile);
		return buffer;
	}

	static char* index_html = nullptr;
	static char* dashboard_html = nullptr;

	WebServer::WebServer(const std::reference_wrapper<QEventChannels>& qc) : qc(qc) {
		index_html = loadFile("index.html");
		dashboard_html = loadFile("dashboard.html");
		WebServer::tcpServer = Intrusive<TCPServer>::make();
		qc.get().qBus.subscribe<event::services::Signal>([this](const event::services::Signal& e) {

			switch (e.signal) {
			case event::services::Signal::Type::Pause:
			case event::services::Signal::Type::Terminate:
			{
				WebServer::bIsServiceRunning = false;
				WebServer::tcpServer->stop();

				while (!WebServer::bIsServiceTerminated) {
					// wait for the service to terminate
				}
			}break;
			case event::services::Signal::Type::Restart:
			{
				WebServer::bIsServiceRunning = false;
				WebServer::tcpServer->stop();

				while (!WebServer::bIsServiceTerminated) {

				}

				//restart
				WebServer::bIsServiceRunning = true;
				WebServer::tcpServer->listen();
			}break;

			case event::services::Signal::Type::Launch:
			{
				WebServer::instantiateService();
				WebServer::runService();
			}break;

			default:
				break;
			}
			});
	}

	WebServer::~WebServer() {
		if (index_html)
			free(index_html);
		if (dashboard_html)
			free(dashboard_html);
	}

	bool WebServer::instantiateService() {
		bIsServiceRunning = (WebServer::tcpServer->listen(8080, "192.168.50.45") != Socket::Status::Error);
		return bIsServiceRunning;
	}

	bool WebServer::runService() {

		std::thread([this](){
			WebServer::tcpWebServiceProc(this);
			}).detach();

		return false;
	}
	bool WebServer::isServiceRunning() const {
		return WebServer::bIsServiceRunning;
	}
	void WebServer::haltService() {
		
	}
	void WebServer::tcpWebServiceProc(const Intrusive<WebServer>& self) {
		
		bool& isServiceRunning = self->bIsServiceRunning;

		char req[4096]{};
		static int refresh = 0;
		while (isServiceRunning) {
			
			TCPClient client(self->qc.get().qLogBus);

			Socket::Status status = safe_access_intr(self->tcpServer, [&client](const Intrusive<TCPServer>& tcpServer) {
				return tcpServer->accept(client);
				}, Socket::Status::Error);

			if (status == Socket::Status::Error) {
				// something badly has happend
				// either a faulty client connected or main service issuing a Terminate Signal
				if (isServiceRunning == false) {
					//Main service is issuing to end the Web service
					self->bIsServiceTerminated = true;
					return;
				}
			}
			eastl::vector<i8>;
			Http req;
			client.receive(req);
			Sleep(1000);
			if (req.getURL() == "/") {
				Http resp;
				resp.setVersion("HTTP/1.1");
				resp.setResponseCode(200, "OK!");
				resp.setBody(index_html);
				client.send(resp);
			}
			else {
				Http resp;
				resp.setVersion("HTTP/1.1");
				resp.setResponseCode(404, "NOT FOUND!");
				resp.setBody(dashboard_html);
				client.send(resp);
			}

			client.disconnect();
		}
		self->bIsServiceTerminated = true;
	}
}