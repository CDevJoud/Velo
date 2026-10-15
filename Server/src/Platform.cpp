#include "Platform.hpp"
#include <atomic>
#if defined(VELO_PLATFORM_WINDOWS)
#include <windows.h>
#else
#include <csignal>
#include <ctime>
#include <cerrno>
#include <unistd.h>
#include <termios.h>
#include <sys/select.h>
#endif
namespace velo::platform {
#if defined(VELO_PLATFORM_WINDOWS)
	void sleepMs(u32 ms) {
		Sleep(ms);
	}
	bool keyPressed(i32 key) {
		return (GetAsyncKeyState(key) & 0x8000) != 0;
	}
	bool stopRequested() {
		return false;
	}
#else
	namespace {
		volatile std::sig_atomic_t stopFlag = 0;
		extern "C" void onStopSignal(int) {
			stopFlag = 1;
		}
		struct SignalSetup {
			SignalSetup() {
				std::signal(SIGPIPE, SIG_IGN);
				struct sigaction action {};
				action.sa_handler = onStopSignal;
				action.sa_flags = SA_RESTART;
				sigemptyset(&action.sa_mask);
				sigaction(SIGINT, &action, nullptr);
				sigaction(SIGTERM, &action, nullptr);
			}
		} signalSetup;
		struct Terminal {
			termios saved{};
			bool active = false;
			bool unavailable = false;
			~Terminal() {
				if (active) {
					tcsetattr(STDIN_FILENO, TCSANOW, &saved);
				}
			}
			bool isForeground() const {
				const pid_t group = tcgetpgrp(STDIN_FILENO);
				return group != -1 && group == getpgrp();
			}
			bool enable() {
				if (active) {
					return isForeground();
				}
				if (unavailable) {
					return false;
				}
				if (!isatty(STDIN_FILENO) || tcgetattr(STDIN_FILENO, &saved) != 0) {
					unavailable = true;
					return false;
				}
				if (!isForeground()) {
					return false;
				}
				termios raw = saved;
				raw.c_lflag &= ~static_cast<tcflag_t>(ICANON | ECHO);
				raw.c_cc[VMIN] = 0;
				raw.c_cc[VTIME] = 0;
				active = tcsetattr(STDIN_FILENO, TCSANOW, &raw) == 0;
				unavailable = !active;
				return active;
			}
			bool readable() const {
				fd_set readSet;
				FD_ZERO(&readSet);
				FD_SET(STDIN_FILENO, &readSet);
				timeval timeout{};
				return ::select(STDIN_FILENO + 1, &readSet, nullptr, nullptr, &timeout) > 0;
			}
		} terminal;
	}
	void sleepMs(u32 ms) {
		timespec request{};
		request.tv_sec = static_cast<time_t>(ms / 1000);
		request.tv_nsec = static_cast<long>(ms % 1000) * 1000000L;
		timespec remain{};
		while (nanosleep(&request, &remain) == -1 && errno == EINTR) {
			if (stopFlag) {
				return;
			}
			request = remain;
		}
	}
	bool keyPressed(i32 key) {
		if (!terminal.enable() || !terminal.readable()) {
			return false;
		}
		unsigned char c = 0;
		if (::read(STDIN_FILENO, &c, 1) != 1) {
			return false;
		}
		if (c == KeyEscape && terminal.readable()) {
			unsigned char discard = 0;
			while (terminal.readable() && ::read(STDIN_FILENO, &discard, 1) == 1) {
			}
			return false;
		}
		return c == static_cast<unsigned char>(key);
	}
	bool stopRequested() {
		return stopFlag != 0;
	}
#endif
}
