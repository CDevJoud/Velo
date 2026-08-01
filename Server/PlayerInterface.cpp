#include "PlayerInterface.hpp"

namespace velo{
	PlayerInterface::PlayerInterface(
		const Intrusive<TCPClient>& client, 
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
	bool PlayerInterface::onPlayerConnect(const Intrusive<PlayerInterface>& player) {
		return false;
	}
	bool PlayerInterface::onPlayerDisconnect(const Intrusive<PlayerInterface>& player) {
		return false;
	}
	bool PlayerInterface::onPlayerJoin(Intrusive<World>& world) {
		return false;
	}
    bool PlayerInterface::onPlayerQuit(const Intrusive<World>& world) {
        return false;
    }
	const Intrusive<TCPClient>& PlayerInterface::getTCPClient() const {
		return this->client;
	}
	const std::reference_wrapper<ServerInterface>& PlayerInterface::getServerInterface() const {
		return this->serverInterface;
	}
	const std::reference_wrapper<QEventBus>& PlayerInterface::getQEventBus() const {
		return this->qBus;
	}
	const std::reference_wrapper<QEventBus>& PlayerInterface::getQEventLogBus() const {
		return this->qLogBus;
	}
	std::u16string PlayerInterface::getUsername() const {
		return this->username;
	}
}
