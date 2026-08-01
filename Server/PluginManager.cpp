#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "PluginManager.hpp"
#include <format>

#define LOG_INFO(msg)  qLogBus.get().post(event::Log(event::Log::Severity::INFO, msg));
#define LOG_DEBUG(msg) qLogBus.get().post(event::Log(event::Log::Severity::DEBUG, msg));
#define LOG_WARN(msg)  qLogBus.get().post(event::Log(event::Log::Severity::WARN, msg));
#define LOG_ERROR(msg) qLogBus.get().post(event::Log(event::Log::Severity::ERROR, msg));
#define LOG_FATAL(msg) qLogBus.get().post(event::Log(event::Log::Severity::FATAL, msg));
#define LOG_INFO_TRACE(msg)  qLogBus.get().post(event::Log(event::Log::Severity::INFO, msg, "default", std::source_location::current()));
#define LOG_DEBUG_TRACE(msg) qLogBus.get().post(event::Log(event::Log::Severity::DEBUG, msg, "default", std::source_location::current()));
#define LOG_WARN_TRACE(msg)  qLogBus.get().post(event::Log(event::Log::Severity::WARN, msg, "default", std::source_location::current()));
#define LOG_ERROR_TRACE(msg) qLogBus.get().post(event::Log(event::Log::Severity::ERROR, msg, "default", std::source_location::current()));
#define LOG_FATAL_TRACE(msg) qLogBus.get().post(event::Log(event::Log::Severity::FATAL, msg, "default", std::source_location::current()));

namespace velo {
	Qword hashMemory(const void* data, size_t size) {
		const Byte* bytes = reinterpret_cast<const Byte*>(data);
		Qword hash = 1469598103934665603ULL;

		for (size_t i = 0; i < size; ++i) {
			hash ^= bytes[i];
			hash *= 1099511628211ULL;
		}

		return hash;
	}
	Xuint64 _hash_str(Xcstr str) {
		Xuint64 h = 0xcbf29ce484222325ULL; // FNV offset basis

		//I believe this is not safe! but whatever.
		while (*str) {
			h ^= (Xuint8)(*str++);
			h *= 0x100000001b3ULL; // FNV prime
		}

		// Final mixing (improves avalanche)
		h ^= h >> 33;
		h *= 0xff51afd7ed558ccdULL;
		h ^= h >> 33;
		h *= 0xc4ceb9fe1a85ec53ULL;
		h ^= h >> 33;

		return h;
	}

	size_t PluginManager::PluginIdentityKeyHash::operator()(const PluginIdentityKey& k) const {
		size_t h = 1469598103934665603ULL;
		h ^= k.nameHash + 0x9E3779B9 + (h << 6) + (h >> 2);
		h ^= k.sessionHash + 0x9E3779B9 + (h << 6) + (h >> 2);

		return h;
	}

	bool PluginManager::PluginIdentityKeyEqual::operator()(const PluginIdentityKey a, const PluginIdentityKey b) const {
		return (a.nameHash == b.nameHash && a.sessionHash == b.sessionHash);
	}

	PluginManager::PluginManager(const std::reference_wrapper<QEventBus>& qLogBus) : qLogBus(qLogBus) {
		
	}
	bool PluginManager::loadPlugin(const std::string& name) {
		using plm = PluginManager;

		LOG_INFO(std::format("[PluginManager] load following plugin {} ...", name));

		HMODULE hModule = LoadLibraryA(name.c_str());

		if (hModule == INVALID_HANDLE_VALUE) {
			LOG_ERROR(std::format("[PluginManager] Plugin '{}' could not be loaded or located!", name));
			return false;
		}

		typedef Xconst XAPIDescriptor(*XI_queryFn)(Xvoid);

		XI_queryFn XI_query = (XI_queryFn)GetProcAddress(hModule, "XI_query");

		if (XI_query == nullptr) {
			LOG_ERROR(std::format("[PluginManager] Could not find 'XI_query' functor, the binary file '{}' is not a valid velo plugin", name));
			FreeLibrary(hModule);
			return false;
		}
		
		XAPIDescriptor plDesc = XI_query();

		std::string plName = plDesc.name;

		Intrusive<PluginInterface> plugin = dynamicPtrCast<PluginInterface>(Intrusive<PluginSession>::make(plDesc, hModule));
		
		if (plugin == nullptr) {
			LOG_FATAL_TRACE(std::format("@velo Internal system error! Could not create a new plugin instance. Is the system running out of memory?"));
			FreeLibrary(hModule);
			return false;
		}

		if (!plugin->instantiate()) {
			LOG_ERROR(std::format("[PluginManager] The following plugin {} seems to not to be a valid velo plugin. Ignoring it!", plName));
			FreeLibrary(hModule);
			return false;
		}

		PluginIdentity plI(_hash_str(plDesc.name), plName);

		PluginIdentityKey plIKey;
		plIKey.nameHash = hashMemory(plI.name.data(), plI.name.length());
		plIKey.sessionHash = plI.qwSession;

		plm::plugins[plIKey] = plugin;

		return true;
	}
	void PluginManager::callOnClientConnect(Intrusive<TCPClient>& client) {
		using plm = PluginManager;

		for (auto& plugin : plm::plugins) {
			if (plugin.second->onClientConnect(client)) {
				LOG_ERROR(std::format("[PluginManager] Plugin {} returned false when calling onClientConnect", plugin.second->desc.name));
				// request for plugin last error!
			}
		}
	}
}
