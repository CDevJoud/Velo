#pragma once

#include "PlayerInterface.hpp"

namespace velo {
	class World;
	class Player : public PlayerInterface{
	public:
		Player(
			const std::shared_ptr<TCPClient>& client,
			const Int32 entityID,
			const std::u16string& username,
			const std::reference_wrapper<QEventBus>& qBus,
			const std::reference_wrapper<ServerInterface>& serverInterface);

		~Player();

		virtual bool onPlayerConnect(const std::shared_ptr<PlayerInterface>& player) override;
		virtual bool onPlayerDisconnect(const std::shared_ptr<PlayerInterface>& player) override;
		virtual bool onPlayerJoin(std::shared_ptr<World>& world) override final;

		std::reference_wrapper<LCEServer>& getServer();

		static void handleConnection(const std::shared_ptr<Player>& instance);

		void join(const std::shared_ptr<World>& world);
	private:
		friend class LCEServer;
		//void addSelf(const std::shared_ptr<Player>& self);
		SubscriptionToken qEvent_playerConnect, qEvent_playerDisconnect;
	};
}
