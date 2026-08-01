#include "ServerInterface.hpp"
#include "PlayerInterface.hpp"

namespace velo {
	ServerInterface::ServerInterface(bool isServerRunning) {
		
	}
	bool ServerInterface::onClientConnect(Intrusive<TCPClient>& client, std::u16string& clientUsername) {
		return false;
	}
	bool ServerInterface::onClientDisconnect(Intrusive<TCPClient>& client, std::u16string& clientUsername) {
		return false;
	}
	bool ServerInterface::isServerRunning() const {
		return ServerInterface::bIsServerRunning;
	}
	bool ServerInterface::isServerQuiting() const {
		return ServerInterface::bIsServerQuiting;
	}
    Intrusive<PlayerInterface> ServerInterface::getPlayer(const std::u16string& username) {
		auto element = ServerInterface::players.find(username);
		if (element != ServerInterface::players.end()) {
			if (auto player = element->second) {
				return player;
			}
		}
		return nullptr;
    }
	bool ServerInterface::insertPlayer(const std::u16string& username, const Intrusive<PlayerInterface>& player) {
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
