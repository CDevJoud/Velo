#include "Player.hpp"
#include "Packet.hpp"
#include <iostream>
#include "LCEServer.hpp"
#include "World.hpp"

#include "Accessors.hpp"

#define LOG_INFO(msg)  qLogBus.get().post(event::Log(event::Log::Severity::INFO, msg));
#define LOG_DEBUG(msg) qLogBus.get().post(event::Log(event::Log::Severity::DEBUG, msg));
#define LOG_WARN(msg)  qLogBus.get().post(event::Log(event::Log::Severity::WARN, msg));
#define LOG_ERROR(msg) qLogBus.get().post(event::Log(event::Log::Severity::ERROR, msg));
#define LOG_FATAL(msg) qLogBus.get().post(event::Log(event::Log::Severity::FATAL, msg));
#define LOG_INFO_TRACE (msg) qLogBus.get().post(event::Log(event::Log::Severity::INFO , msg, "default", std::source_location::current()));
#define LOG_DEBUG_TRACE(msg) qLogBus.get().post(event::Log(event::Log::Severity::DEBUG, msg, "default", std::source_location::current()));
#define LOG_WARN_TRACE (msg) qLogBus.get().post(event::Log(event::Log::Severity::WARN , msg, "default", std::source_location::current()));
#define LOG_ERROR_TRACE(msg) qLogBus.get().post(event::Log(event::Log::Severity::ERROR, msg, "default", std::source_location::current()));
#define LOG_FATAL_TRACE(msg) qLogBus.get().post(event::Log(event::Log::Severity::FATAL, msg, "default", std::source_location::current()));


namespace velo {
	Player::Player(
		const std::shared_ptr<TCPClient>& client,
		const Int32 entityID,
		const std::u16string& username,
		const std::reference_wrapper<QEventBus>& qBus,
		const std::reference_wrapper<QEventBus>& qLogBus,
		const std::reference_wrapper<ServerInterface>& serverInterface) :
		PlayerInterface(client, entityID, username, qBus, qLogBus, serverInterface) {

		// in the future it needs to override existing impl!
		// some what like a static function
		qEvent_playerConnect = qBus.get().subscribeRAII<event::player::Connect>([](const event::player::Connect& e) {
			std::shared_ptr<PlayerInterface> instance1 = e.server.get().getPlayer(e.username);
			std::shared_ptr<PlayerInterface> instance2 = e.player;
			
			if (instance1 == instance2) { // its correct player and not an empty data
				bool ret = instance1->onPlayerConnect(instance1);
				if (!ret) {
					// Error
				}
			}
			});
		qEvent_playerDisconnect = qBus.get().subscribeRAII<event::player::Disconnect>([](const event::player::Disconnect& e) {
			const std::shared_ptr<PlayerInterface>& instance1 = e.server.get().getPlayer(e.username);
			const std::shared_ptr<PlayerInterface>& instance2 = e.player;

			std::shared_ptr<Player> player = nullptr;
			std::shared_ptr<TCPClient> tcpClient = nullptr;

			if (instance1 == instance2) {
				auto& qLogBus = instance1->getQEventLogBus();
				player = std::dynamic_pointer_cast<Player>(instance1);
				if (player != nullptr) {
					tcpClient = player->getTCPClient();
				}
				bool ret = instance1->onPlayerDisconnect(instance1);
				if (!ret) {
					// Error
				}

				// Remove the player from the server registries!
				// Even though we removed the player from registry
				// the player object still exist as the ref count is
				// not zero so `player` and `tcpClient` is not nullptr yet!
				// This basically makes it that the player could not be accessed
				// with it's key username!
				if (e.server.get().removePlayer(e.username)) {
					// removed the player from registries
				}
				else {
					// couldn't remove the player! the player may be already removed! How??!
				}

				//post a client disconnect event
				if (player != nullptr && tcpClient != nullptr) {
					instance1->getQEventBus().get().post<event::client::Disconnect>(event::client::Disconnect(
						tcpClient,
						player->getServer()
					));
				}
				else {
					LOG_ERROR_TRACE("[CRITICAL] Attempted access to nullptr detected. Operation aborted safely to prevent segmentation fault and maintain application stability.");
				}
			}
		});
	}

	Player::~Player() {

	}

	bool Player::onPlayerConnect(const std::shared_ptr<PlayerInterface>& player) {
		std::shared_ptr<Player> instance = std::dynamic_pointer_cast<Player>(player);

		std::u16string username = instance->getUsername();

		auto& qLogBus = instance->getQEventLogBus();
		std::string username8(username.begin(), username.end());
		LOG_INFO("@velo: Player " + username8 + " connected to the server!");
		
		std::shared_ptr<World> world = instance->getServer().get().getWorld();
		if (world == nullptr) {
			return false;
		}

		LOG_INFO("@velo: Player joining " + world->getName());
		instance->join(instance->getServer().get().getWorld());

		return true;
	}

	bool Player::onPlayerDisconnect(const std::shared_ptr<PlayerInterface>& player) {
		auto instance = (Player*)player.get();
		if (instance != nullptr) {
			auto& qLogBus = instance->getQEventLogBus();

			std::u16string username = instance->getUsername();
			std::string username8(username.begin(), username.end());

			LOG_INFO("[Player] " + username8 + " disconnected from the server");
			return true;
		}
		return false;
	}

	bool Player::onPlayerJoin(std::shared_ptr<World>& world) {
		return true;
	}

	bool Player::onPlayerQuit(const std::shared_ptr<World>& world) {
		return true;
	}

	std::reference_wrapper<LCEServer>& Player::getServer() {
		return *(std::reference_wrapper<LCEServer>*)(&Player::getServerInterface());
	}

	void Player::handleConnection(const std::shared_ptr<Player>& _) {
		std::shared_ptr<Player> instance = _; // keep me alive!
		instance->self = _; // also keep me alive!

		Int32 countKeepAlive = 0;
		auto& qLogBus = instance->self->getQEventLogBus();
		std::u16string username = instance->self->getUsername();
		std::string username8(username.begin(), username.end());
		while (safe_access<bool>(instance, [](const auto& _instance) -> bool {return _instance->getServerInterface().get().isServerRunning();}, false)) {
			Packet req = Packet::ID::Invalid;

			auto res = safe_access<Socket::Status>(instance, [&req](const auto& _instance) {
				return safe_access<Socket::Status>(_instance->getTCPClient(), [&req](const auto& client) {
					return client->receive(req);
					});
				});

			if (res == Socket::Disconnected || res == Socket::Error) {
				break;
			}
			Packet::ID pid = req.getID();
			switch (pid) {
			case velo::Packet::ID::KeepAlive:
				LOG_INFO("[Player] " + username8 + " sent keep alive");
				countKeepAlive++;
				break;
			case velo::Packet::ID::Login:
				break;
			case velo::Packet::ID::PreLogin:
				break;
			case velo::Packet::ID::Invalid:
				break;

			case velo::Packet::ID::DebugOptions:
			{
				DebugOptionsPacket dop{};
				req.parsePacket(&dop);
				LOG_INFO("[Player] " + username8 + " sent debug options: " + std::to_string(dop.value));
			}
			default:
				break;
			}
			if (countKeepAlive >= 5) {
				safe_access(instance, [](const std::shared_ptr<Player>& _instance) {
					safe_access(_instance->getTCPClient(), [](const std::shared_ptr<TCPClient>& client) {
						client->send(
							Packet::createDisconnect(
								DisconnectPacket::LoginTooLong
							)
						);
						});
					});
				break;
			}
		}
		// it means just quit from the world
		// and prepare for proper disconnection
		instance->join(nullptr);
		safe_access(instance, [](const std::shared_ptr<Player>& _instance) {
			_instance->getQEventBus().get().post(
				event::player::Disconnect(
					std::dynamic_pointer_cast<PlayerInterface>(_instance),
					_instance->getUsername(),
					_instance->getServer()
				)
			);
			});
		instance->self = nullptr;
		instance = nullptr; // destroy the player instance
	}
	void Player::join(const std::shared_ptr<World>& world) {
		if (Player::world == nullptr) {
			Player::world = world;
			std::shared_ptr<int> s;
			Player::getQEventBus().get().post(
				event::player::Join(
					self,
					world,
					this->getServer()
				)
			);
		}
		else {
			Player::getQEventBus().get().post(
				event::player::Quit(
					self,
					Player::world,
					this->getServer()
				)
			);
			Player::world = world; // store the new world
			Player::getQEventBus().get().post(
				event::player::Join(
					self,
					Player::world,
					this->getServer()
				)
			);
		}
	}
}
