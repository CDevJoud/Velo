#pragma once
#include "PluginSession.hpp"
#include <unordered_map>
namespace velo {
	
	class PluginManager {
	private:
		struct PluginIdentity {
			PluginIdentity() = default;
			PluginIdentity(Qword qwSession, const std::string& name) : qwSession(qwSession), name(name) {

			}
			~PluginIdentity() {

			}
			Qword qwSession;
			std::string name;
		};

		struct PluginIdentityKey {
			Dword sessionHash, nameHash;
		};

		struct PluginIdentityKeyHash {
			size_t operator()(const PluginIdentityKey& k) const;
		};

		struct PluginIdentityKeyEqual {
			bool operator()(const PluginIdentityKey a, const PluginIdentityKey b) const;
		};
	public:
		PluginManager(const std::reference_wrapper<QEventBus>& qLogBus);
		bool loadPlugin(const std::string& name);

		void callOnClientConnect(Intrusive<TCPClient>& client);
	private:
		std::reference_wrapper<QEventBus> qLogBus;
		std::unordered_map<
			PluginIdentityKey, 
			Intrusive<PluginInterface>,
			PluginIdentityKeyHash,
			PluginIdentityKeyEqual
		> plugins;
	};
}

