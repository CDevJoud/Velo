#include "World.hpp"
#include "Player.hpp"
#include "Accessors.hpp"
#include "Packet.hpp"

namespace velo {
	World::World(
		const std::reference_wrapper<QEventBus>& qBus,
		const std::reference_wrapper<LCEServer>& server
	) : server(server), qBus(qBus) {
		QEvent_playerJoin = qBus.get().subscribeRAII<event::player::Join>([this](const event::player::Join& e) {
			std::shared_ptr<World> self = e.world;
			std::shared_ptr<Player> _player = std::dynamic_pointer_cast<Player>(e.player);
			if (self.get() == this) {
				
				if(!safe_access<bool>(_player, [&self](const auto& p) { return p->onPlayerJoin(self);})){
					// Error 
					return;
				}
				auto it = std::ranges::find_if(self->players, [_player](const auto& p) -> bool {
					return p->getUsername() == _player->getUsername();
					});
				// if it does exist
				if (it != this->players.end()) {
					return; // the player already exist!
				}

				auto tcpClient = safe_access<std::shared_ptr<TCPClient>>(_player, [](const auto& instance) { return instance->getTCPClient();});

				// send chunk visibility
				auto res = safe_access<Socket::Status>(tcpClient, [](const auto& client) { return client->send(Packet::createChunkVisibility(0, 0, true));});
				if (res == Socket::Error || res == Socket::Disconnected) {
					return; // Error
				}

				// send other packets...
				
				this->players.push_back(_player);
			}
			});
	}
	World::~World() {
	
	}
}
