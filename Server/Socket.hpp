#pragma once
#include "typedefs.hpp"

namespace velo {
	class Socket {
	public:
		static constexpr inline Qword Invalid = (Qword)(~0);
		enum Status {
			Error = 0,
			Done,
			NotReady,
			Partial,
			Disconnected
		};

		Status getErrorStatus();

		Socket();
		virtual ~Socket();

		Socket(Socket&& other) noexcept;
		Socket& operator=(Socket&& other) noexcept;

		void setBlocking(bool value);
		bool isBlocking() const;

		Qword getNativeHandle() const;
	protected:
		friend class TCPServer;

		void close();
		void create();
		void create(Qword handle);

	private:
		Qword qwSocket;
		bool bIsBlocking;
	};
}