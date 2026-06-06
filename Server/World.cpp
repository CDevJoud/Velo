#include "World.hpp"
#include "Player.hpp"
#include "Accessors.hpp"
#include "Packet.hpp"

#include <iostream>

#define LOG_INFO(msg) World::qLogBus.get().post(event::Log(event::Log::Severity::INFO, msg));
#define LOG_DEBUG(msg) World::qLogBus.get().post(event::Log(event::Log::Severity::DEBUG, msg));
#define LOG_WARN(msg) World::qLogBus.get().post(event::Log(event::Log::Severity::WARN, msg));
#define LOG_ERROR(msg) World::qLogBus.get().post(event::Log(event::Log::Severity::ERROR, msg));
#define LOG_FATAL(msg) World::qLogBus.get().post(event::Log(event::Log::Severity::FATAL, msg));
#define LOG_INFO_TRACE (msg) qBus.post(event::Log(event::Log::Severity::INFO , msg, "default", std::source_location::current()));
#define LOG_DEBUG_TRACE(msg) qBus.post(event::Log(event::Log::Severity::DEBUG, msg, "default", std::source_location::current()));
#define LOG_WARN_TRACE (msg) qBus.post(event::Log(event::Log::Severity::WARN , msg, "default", std::source_location::current()));
#define LOG_ERROR_TRACE(msg) qBus.post(event::Log(event::Log::Severity::ERROR, msg, "default", std::source_location::current()));
#define LOG_FATAL_TRACE(msg) qBus.post(event::Log(event::Log::Severity::FATAL, msg, "default", std::source_location::current()));


namespace velo {
	World::World(
		const std::reference_wrapper<QEventBus>& qBus,
		const std::reference_wrapper<QEventBus>& qLogBus,
		const std::reference_wrapper<LCEServer>& server
	) : server(server), qBus(qBus), qLogBus(qLogBus), name("defaultWorld") {
		QEvent_playerQuit = qBus.get().subscribeRAII<event::player::Quit>([this](const event::player::Quit& e) {
			std::shared_ptr<World> self = e.world;
			std::shared_ptr<Player> _player = std::dynamic_pointer_cast<Player>(e.player);
			
			std::u16string playerUsername = _player->getUsername();
			std::string username8(playerUsername.begin(), playerUsername.end());
			LOG_DEBUG("Reference count of " + username8 + " set to " + std::to_string(_player.use_count()));
			if (self.get() == this) {
				if (!safe_access<bool>(_player, [&self](const auto& p) { return p->onPlayerQuit(self); })) {
					return; // player.onPlayerQuit() refused the quit event from this world!
				}
				auto it = std::find_if(self->players.begin(), self->players.end(), [_player](const auto& p) -> bool {
					return p.lock()->getUsername() == _player->getUsername();
					});
				// if it does exist
				if (it != this->players.end()) {
					// handle world quitting properly later...
					self->players.erase(it);
				}
				else {
					// player does not exist in this world!
					LOG_ERROR(username8 + " does not exist in " + self->name);
				}
			}
			});
		QEvent_playerJoin = qBus.get().subscribeRAII<event::player::Join>([this](const event::player::Join& e) {
			std::shared_ptr<World> self = e.world;
			std::shared_ptr<Player> _player = std::dynamic_pointer_cast<Player>(e.player);
			
			if (self.get() == this && _player != nullptr) {
				std::u16string playerUsername = _player->getUsername();
				std::string username8(playerUsername.begin(), playerUsername.end());

				LOG_DEBUG("Reference count of " + username8 + " set to " + std::to_string(_player.use_count()));


				if(!safe_access<bool>(_player, [&self](const auto& p) { return p->onPlayerJoin(self);})){
					// Error 
					LOG_ERROR("Player::onPlayerJoin for " + username8 + " refused joining " + self->name);
					return;
				}
				auto it = std::find_if(self->players.begin(), self->players.end(), [_player](const auto& p) -> bool {
					return p.lock()->getUsername() == _player->getUsername();
					});
				// if it does exist
				if (it != this->players.end()) {
					LOG_INFO(username8 + "already joined " + self->name);
					return; // the player already exist!
				}

				auto tcpClient = safe_access<std::shared_ptr<TCPClient>>(_player, [](const auto& instance) { return instance->getTCPClient();});

				// send chunk visibility
				auto res = safe_access<Socket::Status>(tcpClient, [](const auto& client) { return client->send(Packet::createChunkVisibility(0, 0, true));});
				if (res == Socket::Error || res == Socket::Disconnected) {
					LOG_ERROR("[World:" + self->name + "] I tried sending chunk visibility to " + username8 + " but it seems that the client disconnected!\n\tPreparing on handlings player disconnection!");
					return; // Error
				}

				

				// send other packets...
				
				this->players.emplace_back(_player);
				LOG_INFO("[World:" + self->name + "] " + username8 + " joined the world!");
			}
			});
	}
	World::~World() {
	
	}
	std::string World::getName() const {
		return this->name;
	}
}

