#pragma once
#include "ServerInterface.hpp"
#include "TCPServer.hpp"
#include "QEventBus.hpp"
#include "World.hpp"

namespace velo {
	class LCEServer : public ServerInterface, private TCPServer {
	public:
		LCEServer();
		~LCEServer();
		virtual bool onClientConnect(const std::shared_ptr<TCPClient>& client, std::u16string& clientUsername) override;
		virtual bool onClientDisconnect(const std::shared_ptr<TCPClient>& client, std::u16string& clientUsername) override;

		struct Config {
			Int32 maxPlayers = 20;
			std::string worldName = "world";
			std::string ipAddress = "127.0.0.1";
			Word port = 25565;
		};

		Config& getConfig();

		Int32 runService();

		std::shared_ptr<World>& getWorld();
	private:
		inline void initQEventBusSubscriptions();
		__forceinline void handleIncomingConnection(TCPClient& client);
		Config cfg;
		QEventBus qBus;
		std::shared_ptr<World> world;
	};
}

