#include "ServerInterface.hpp"

namespace velo {
	ServerInterface::ServerInterface(bool isServerRunning) {
		
	}
	bool ServerInterface::onClientConnect(const std::shared_ptr<TCPClient>& client, std::u16string& clientUsername) {
		return false;
	}
	bool ServerInterface::onClientDisconnect(const std::shared_ptr<TCPClient>& client, std::u16string& clientUsername) {
		return false;
	}
	bool ServerInterface::isServerRunning() const {
		return ServerInterface::bIsServerRunning;
	}
	bool ServerInterface::isServerQuiting() const {
		return ServerInterface::bIsServerQuiting;
	}
    std::shared_ptr<PlayerInterface> ServerInterface::getPlayer(const std::u16string& username) {
		auto element = ServerInterface::players.find(username);
		if (element != ServerInterface::players.end()) {
			if (auto player = element->second.lock()) {
				return player;
			}
		}
		return nullptr;
    }
	bool ServerInterface::insertPlayer(const std::u16string& username, const std::shared_ptr<PlayerInterface>& player) {
		auto element = ServerInterface::players.find(username);
		if (element == ServerInterface::players.end()) {
			ServerInterface::players[username] = player;
			return true;
		}
		return false;
	}
	bool ServerInterface::removePlayer(const std::u16string& username) {
		auto p = ServerInterface::players.find(username);
		if (p != ServerInterface::players.end()) {
			ServerInterface::players.erase(p);
			return true;
		}
		return false;
	}
}
