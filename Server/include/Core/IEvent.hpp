#pragma once

namespace velo {
	struct IEvent {
		[[nodiscard]] virtual const char* name() const = 0;
	};

	namespace event {
		namespace services {
			struct Signal : IEvent {
				[[nodiscard]] virtual const char* name() const override final { return "velo.event.services.halt_signal"; }

				enum class Type {
					Terminate = 0,
					Launch,
					Pause,
					Restart
				};

				Signal(Type signal = Type::Terminate) : signal(signal) {}

				Type signal;
			};
		}
	}
}
