#include "PlayerInterface.hpp"

namespace velo{
	PlayerInterface::PlayerInterface(
		const std::shared_ptr<TCPClient>& client, 
		const Int32 entityID, 
		const std::u16string& username, 
		const std::reference_wrapper<QEventBus>& qBus, 
		const std::reference_wrapper<QEventBus>& qLogBus, 
		const std::reference_wrapper<ServerInterface>& serverInterface) : 
		qBus(qBus),
		qLogBus(qLogBus),
		serverInterface(serverInterface),
		username(username),
		entityID(entityID),
		client(client) {
		
	}

	PlayerInterface::~PlayerInterface() {
	
	}
	bool PlayerInterface::onPlayerConnect(const std::shared_ptr<PlayerInterface>& player) {
		return false;
	}
	bool PlayerInterface::onPlayerDisconnect(const std::shared_ptr<PlayerInterface>& player) {
		return false;
	}
	bool PlayerInterface::onPlayerJoin(std::shared_ptr<World>& world) {
		return false;
	}
    bool PlayerInterface::onPlayerQuit(const std::shared_ptr<World>& world) {
        return false;
    }
	std::shared_ptr<TCPClient>& PlayerInterface::getTCPClient() {
		return this->client;
	}
	const std::reference_wrapper<ServerInterface>& PlayerInterface::getServerInterface() {
		return this->serverInterface;
	}
	std::reference_wrapper<QEventBus>& PlayerInterface::getQEventBus() {
		return this->qBus;
	}
	std::reference_wrapper<QEventBus>& PlayerInterface::getQEventLogBus() {
		return this->qLogBus;
	}
	std::u16string PlayerInterface::getUsername() const {
		return this->username;
	}
}
