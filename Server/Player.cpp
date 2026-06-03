#include "Player.hpp"
#include "Packet.hpp"
#include <iostream>
#include "LCEServer.hpp"
#include "World.hpp"

#include "Accessors.hpp"

namespace velo {
	Player::Player(
		const std::shared_ptr<TCPClient>& client,
		const Int32 entityID,
		const std::u16string& username,
		const std::reference_wrapper<QEventBus>& qBus,
		const std::reference_wrapper<ServerInterface>& serverInterface) :
		PlayerInterface(client, entityID, username, qBus, serverInterface) {

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
					std::cout << "[CRITICAL] Attempted access to nullptr detected. Operation aborted safely to prevent segmentation fault and maintain application stability." << std::endl;
				}
			}
		});
	}

	Player::~Player() {

	}

	bool Player::onPlayerConnect(const std::shared_ptr<PlayerInterface>& player) {
		std::shared_ptr<Player> instance = std::dynamic_pointer_cast<Player>(player);
		std::cout << "Player ";
		std::u16string username = safe_access<std::u16string>(instance, [](const std::shared_ptr<Player>& p) { return p->getUsername(); });
		if (username.empty()) {
			return false; // username could not be found 
		}
		for (int i = 0; i < username.length(); i++) {
			std::cout << static_cast<char>(player->getUsername()[i]);
		}
		std::cout << " connected to the server!\n";

		instance->join(instance->getServer().get().getWorld());

		return true;
	}

	bool Player::onPlayerDisconnect(const std::shared_ptr<PlayerInterface>& player) {
		auto instance = (Player*)player.get();
		if (instance != nullptr) {
			std::cout << "Player ";
			for (int i = 0; i < player->getUsername().length(); i++) {
				std::cout << static_cast<char>(player->getUsername()[i]);
			}
			std::cout << " disconnected from the server!\n";
			return true;
		}
		return false;
	}

	bool Player::onPlayerJoin(std::shared_ptr<World>& world) {
		return true;
	}

	std::reference_wrapper<LCEServer>& Player::getServer() {
		return *(std::reference_wrapper<LCEServer>*)(&Player::getServerInterface());
	}

	void Player::handleConnection(const std::shared_ptr<Player>& _) {
		std::shared_ptr<Player> instance = _; // keep me alive!

		//for now just send the world data in this way!
		/*safe_access(instance, [](const std::shared_ptr<Player>& _instance) {
			safe_access(_instance->getTCPClient(), [](const std::shared_ptr<TCPClient>& client) {
				client->send(Packet::createChunkVisibility(0, 0, true));
				});
			});*/

		Int32 countKeepAlive = 0;

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
				std::cout << "Player sent Keep Alive" << std::endl;
				std::cout << "Times player sent keep alive: " << countKeepAlive++ << std::endl;
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
				std::cout << "Player sent Debug Options: " << dop.value << std::endl;
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
		safe_access(instance, [](const std::shared_ptr<Player>& _instance) {
			_instance->getQEventBus().get().post(
				event::player::Disconnect(
					std::dynamic_pointer_cast<PlayerInterface>(_instance),
					_instance->getUsername(),
					_instance->getServer()
				)
			);
			});
		instance = nullptr; // destroy the player instance
	}
	void Player::join(const std::shared_ptr<World>& world) {
		Player::getQEventBus().get().post(
			event::player::Join(
				std::shared_ptr<PlayerInterface>(this),
				this->getServer().get().getWorld(),
				this->getServer()
			)
		);
	}
}
