#pragma once
#include "XVAPI_Impl.hpp"
#include "TCPClient.hpp"

namespace velo {
	class PluginInterface : public IntrusiveCounted {
	public:
		PluginInterface(XAPIDescriptor desc, Xvoid* hModule);

		XAPIDescriptor getPluginDescriptor() const { return desc; }

		virtual bool onClientConnect(Intrusive<TCPClient>& client) = 0;
		virtual bool onClientDisconnect(Intrusive<TCPClient>& client) = 0;

	protected:
		virtual bool instantiate();
	private:
		friend class PluginManager;

		XAPIDescriptor desc;
		Xvoid* hModule;

		typedef Xint32(*XI_mainFn)(Xvoid*);
		typedef Xint32(*XI_terminateFn)(Xvoid*);
		XI_mainFn XI_main;
		XI_terminateFn XI_terminate;
	};
}
