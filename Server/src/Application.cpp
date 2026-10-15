#include "Application.hpp"
#include "Platform.hpp"
#include <EASTL/fixed_vector.h>

//@todo implement logging system on TCPClient

using velo::net::TCPServer;
using velo::net::TCPClient;
using velo::core::Intrusive;
using velo::services::WebServer;
using eastl::fixed_vector;
using eastl::vector;


namespace velo {
	Application::Application() : qLogBus("loggerBus"), qBus("default"), qc(qLogBus, qBus){
		qc.runAsync();
		Application::webserver = Intrusive<WebServer>::make(qc);
	}

	i32 Application::main() {
		qc.qBus.post(event::services::Signal(event::services::Signal::Type::Launch));

		while (1) {

			if (platform::keyPressed(platform::KeyEscape) || platform::stopRequested()) {
				break;
			}
			platform::sleepMs(1);
		}

		qc.qBus.post(event::services::Signal());

		Application::waitForServices();

		qc.stop();
		return 0;
	}
	void Application::waitForServices() {
		while (Application::webserver->isServiceRunning()) {

		}
	}
}

int main() {
	velo::Application app;
	return app.main();
}

