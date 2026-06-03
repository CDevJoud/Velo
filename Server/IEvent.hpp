#pragma once

#include <memory>
#define IEVENT_DECLARE_EVENT_NAME(_name) virtual const char const* name() const override { return "velo.event" _name; }

namespace velo {
	class TCPClient;
	class LCEServer;
	class Player;
	class World;
	class PlayerInterface;
	struct IEvent {
		virtual ~IEvent() {}
		[[nodiscard]] virtual const char const* name() const = 0;
	};

	namespace event {
		namespace client {
			struct Connect : public IEvent {
				IEVENT_DECLARE_EVENT_NAME("client.connect")
				Connect(
					const std::shared_ptr<TCPClient>& client, 
					const std::reference_wrapper<LCEServer>& server
				) : tcpClient(client), server(server) {}
				std::shared_ptr<TCPClient> tcpClient;
				std::reference_wrapper<LCEServer> server;
			};
			struct Disconnect : public IEvent {
				IEVENT_DECLARE_EVENT_NAME("client.disconnect")
				Disconnect(
					const std::shared_ptr<TCPClient>& client, 
					const std::reference_wrapper<LCEServer>& server
				) : tcpClient(client), server(server) {}
				std::shared_ptr<TCPClient> tcpClient;
				std::reference_wrapper<LCEServer> server;
			};
		}
		namespace player {
			struct Connect : public IEvent {
				IEVENT_DECLARE_EVENT_NAME("player.connect")
				Connect(
					const std::shared_ptr<PlayerInterface>& player, 
					const std::u16string& username, 
					const std::reference_wrapper<LCEServer>& server
				) : player(player), username(username), server(server) {}
				std::shared_ptr<PlayerInterface> player;
				std::reference_wrapper<LCEServer> server;
				std::u16string username;
			};
			struct Disconnect : public IEvent {
				IEVENT_DECLARE_EVENT_NAME("player.disconnect")
				Disconnect(
					const std::shared_ptr<PlayerInterface>& player, 
					const std::u16string& username, 
					const std::reference_wrapper<LCEServer> server
				) : player(player), username(username), server(server) {}
				std::shared_ptr<PlayerInterface> player;
				std::u16string username;
				std::reference_wrapper<LCEServer> server;
			};

			struct Join : public IEvent {
				IEVENT_DECLARE_EVENT_NAME("player.join")
				Join(
					const std::shared_ptr<PlayerInterface>& player,
					const std::shared_ptr<World>& world,
					const std::reference_wrapper<LCEServer> server
				) : player(player), world(world), server(server) {}

				std::shared_ptr<PlayerInterface> player;
				std::shared_ptr<World> world;
				std::reference_wrapper<LCEServer> server;
			};
		}
	}

}
