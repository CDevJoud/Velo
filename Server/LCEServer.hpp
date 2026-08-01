#pragma once
#include "ServerInterface.hpp"
#include "TCPServer.hpp"
#include "QEventBus.hpp"
#include "World.hpp"
#include "PluginManager.hpp"

namespace velo {
	class Logger;
	class LCEServer : public ServerInterface, private TCPServer{
	public:
		LCEServer();
		~LCEServer();
		virtual bool onClientConnect(Intrusive<TCPClient>& client, std::u16string& clientUsername) override;
		virtual bool onClientDisconnect(Intrusive<TCPClient>& client, std::u16string& clientUsername) override;

		struct Config {
			Int32 maxPlayers = 20;
			std::string worldName = "world";
			std::string ipAddress = "127.0.0.1";
			Word port = 25565;
		};

		Config& getConfig();

		Int32 runService();

		Intrusive<World>& getWorld();
	private:
		inline void initQEventBusSubscriptions();
		__forceinline void handleIncomingConnection(TCPClient& client);
		Config cfg;
		QEventBus qBus, qLogBus;
		Intrusive<World> world;
		std::unique_ptr<PluginManager> plm;
		std::unique_ptr<Logger> logger;
		std::thread tuiThread;
	};
}

