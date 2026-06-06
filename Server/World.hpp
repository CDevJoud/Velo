#pragma once

#include "LCEServer.hpp"

namespace velo {
	class Player;
	class World {
	public:
		World(
			const std::reference_wrapper<QEventBus>& qBus,
			const std::reference_wrapper<QEventBus>& qLogBus,
			const std::reference_wrapper<LCEServer>& server
		);
		~World();

		std::string getName() const;
	private:
		std::vector<std::weak_ptr<Player>> players;
		std::reference_wrapper<LCEServer> server;
		std::reference_wrapper<QEventBus> qBus, qLogBus;
		SubscriptionToken QEvent_playerJoin, QEvent_playerQuit;
		std::string name;
	};
}