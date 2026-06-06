#pragma once

#include "QEventBus.hpp"

namespace velo {
	// @note windows only!
	// @todo implement logger to unix systems
	class Logger {
	public:
		Logger(const std::reference_wrapper<QEventBus> qBus);

		~Logger();
	private:
		void log(const std::string& msg, std::optional<std::source_location> location);
		
		void* hConsoleOut;

		std::mutex mutex;
		std::reference_wrapper<QEventBus> qBus;
	};
}

