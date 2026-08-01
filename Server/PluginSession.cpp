#include "PluginSession.hpp"


namespace velo {
	PluginSession::PluginSession(XAPIDescriptor desc, Xvoid* hModule) 
	: PluginInterface(desc, hModule) {
		
	}
	bool PluginSession::onClientConnect(Intrusive<TCPClient>& client) {
		return false;
	}

	bool PluginSession::onClientDisconnect(Intrusive<TCPClient>& client) {
		return false;
	}
	
}
