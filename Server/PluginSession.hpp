#pragma once
#include "PluginInterface.hpp"

namespace velo {
	class PluginSession : public PluginInterface {
	public:
		PluginSession(XAPIDescriptor desc, Xvoid* hModule);

		virtual bool onClientConnect(Intrusive<TCPClient>& client) override;
		virtual bool onClientDisconnect(Intrusive<TCPClient>& client) override;
	private:

		class PluginRegistry {
		public:

		};

		XIExports exports;
	};
}
