#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "Logger.hpp"

namespace velo{
	static std::atomic_bool bIsServiceRunning = false;
    Logger::Logger(const std::reference_wrapper<QEventBus> qBus) : qBus(qBus) {
		bIsServiceRunning = true;
		qBus.get().subscribe<event::Log>([this](const event::Log& e) {
			log(static_cast<char>(e.severity) + e.msg, e.location);
			});
#ifdef _WIN32
		hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
		if (hConsoleOut == INVALID_HANDLE_VALUE) {
			AllocConsole();
			hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
			if (hConsoleOut == INVALID_HANDLE_VALUE) {
				MessageBoxA(nullptr, "Could not init a console window!\nThe process will terminate!", "velo -> Fatal", MB_OK | MB_ICONHAND);
				TerminateProcess(GetCurrentProcess(), -1);
			}
		}
#else
#error UNIX based system is not available right now!
#endif
	}
	Logger::~Logger() {
		std::lock_guard<std::mutex> lock(mutex);
	}
	void Logger::log(const std::string& msg, std::optional<std::source_location> location) {
		std::lock_guard<std::mutex> lock(mutex);
		char col = msg[0];
		auto str = msg.data() + 1;

#ifdef _WIN32
		DWORD w;
		SYSTEMTIME sysTime{};
		GetSystemTime(&sysTime);
		SetConsoleTextAttribute(hConsoleOut, col);

		std::string date = "[" + std::to_string(sysTime.wYear) + "/" + std::to_string(sysTime.wMonth) + "/" + std::to_string(sysTime.wDay) + "] ";
		WriteConsoleA(hConsoleOut, date.c_str(), date.length(), &w, 0);

		std::string time = "{" + std::to_string(sysTime.wHour) + ":" + std::to_string(sysTime.wMinute) + ":" + std::to_string(sysTime.wSecond) + "} ";
		WriteConsoleA(hConsoleOut, time.c_str(), time.length(), &w, 0);

		WriteConsoleA(hConsoleOut, str, msg.length() - 1, &w, 0);

		if (location.has_value()) {
			std::string loc = std::string(" -> ") + location.value().file_name() + " " + std::string(location.value().function_name()) + " line:" + std::to_string(location.value().line());
			WriteConsoleA(hConsoleOut, loc.c_str(), loc.length(), &w, 0);
		}

		WriteConsoleW(hConsoleOut, u"\n", 1, &w, 0);
		SetConsoleTextAttribute(hConsoleOut, 0x0F);
#else
#error UNIX based system is not available right now!
#endif
	}
}