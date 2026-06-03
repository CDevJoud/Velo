#pragma once

#include "LCEServer.hpp"

namespace velo {
	class Player;
	class World {
	public:
		World(
			const std::reference_wrapper<QEventBus>& qBus,
			const std::reference_wrapper<LCEServer>& server
		);
		~World();
	private:
		std::vector<std::shared_ptr<Player>> players;
		std::reference_wrapper<LCEServer> server;
		std::reference_wrapper<QEventBus> qBus;
		SubscriptionToken QEvent_playerJoin;
	};
}