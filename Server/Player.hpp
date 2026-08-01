#pragma once

#include "PlayerInterface.hpp"

namespace velo {
	class World;
	class Player : public PlayerInterface {
	public:
		Player(
			const Intrusive<TCPClient>& client,
			const Int32 entityID,
			const std::u16string& username,
			const std::reference_wrapper<QEventBus>& qBus,
			const std::reference_wrapper<QEventBus>& qLogBus,
			const std::reference_wrapper<ServerInterface>& serverInterface);

		~Player();

		virtual bool onPlayerConnect(const Intrusive<PlayerInterface>& player) override;
		virtual bool onPlayerDisconnect(const Intrusive<PlayerInterface>& player) override;
		virtual bool onPlayerJoin(Intrusive<World>& world) override;
		virtual bool onPlayerQuit(const Intrusive<World>& world) override;

		std::reference_wrapper<LCEServer>& getServer();

		static void handleConnection(const Intrusive<Player>& instance);

		void join(const Intrusive<World>& world);
	private:
		friend class LCEServer;
		Intrusive<World> world;// the world the player is playing in.
		SubscriptionToken qEvent_playerConnect, qEvent_playerDisconnect;
		Intrusive<Player> self;
	};

	namespace event::player {
		struct Connect : public IEvent {
			IEVENT_DECLARE_EVENT_NAME("player.connect")
				Connect(
					const Intrusive<PlayerInterface>& player,
					const std::u16string& username,
					const std::reference_wrapper<LCEServer>& server
				) : player(player), username(username), server(server) {}
			Intrusive<PlayerInterface> player;
			std::reference_wrapper<LCEServer> server;
			std::u16string username;
		};
		struct Disconnect : public IEvent {
			IEVENT_DECLARE_EVENT_NAME("player.disconnect")
				Disconnect(
					const Intrusive<PlayerInterface>& player,
					const std::u16string& username,
					const std::reference_wrapper<LCEServer> server
				) : player(player), username(username), server(server) {}
			Intrusive<PlayerInterface> player;
			std::u16string username;
			std::reference_wrapper<LCEServer> server;
		};

		struct Join : public IEvent {
			IEVENT_DECLARE_EVENT_NAME("player.join")
				Join(
					const Intrusive<PlayerInterface>& player,
					const Intrusive<World>& world,
					const std::reference_wrapper<LCEServer> server
				) : player(player), world(world), server(server) {}

			Intrusive<PlayerInterface> player;
			Intrusive<World> world;
			std::reference_wrapper<LCEServer> server;
		};

		struct Quit : public IEvent {
			IEVENT_DECLARE_EVENT_NAME("player.quit")
				Quit(
					const Intrusive<PlayerInterface>& player,
					const Intrusive<World>& world,
					const std::reference_wrapper<LCEServer> server
					//const std::function<void()> denied_request_callback = nullptr
				) : player(player), world(world), server(server) /*deniedReqCB(denied_request_callback)*/ {}

			Intrusive<PlayerInterface> player;
			Intrusive<World> world;
			std::reference_wrapper<LCEServer> server;
			//std::function<void()> deniedReqCB;
		};
	}
}
