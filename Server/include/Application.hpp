#pragma once
#include <EASTL/unique_ptr.h>

#include "Services/WebServer/WebServer.hpp"

namespace velo {
	class Application {
	public:
		Application();

		i32 main();
	private:
		void waitForServices();
		core::Intrusive<services::WebServer> webserver = nullptr;
		core::QEventBus qBus;
		core::QEventBus qLogBus;
		core::QEventChannels qc;
	};
}