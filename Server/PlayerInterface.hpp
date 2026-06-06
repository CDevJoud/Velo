#pragma once
#include "TCPClient.hpp"
#include <memory>
#include "QEventBus.hpp"
#include "ServerInterface.hpp"

namespace velo {
	class PlayerInterface {
	public:
		PlayerInterface(
			const std::shared_ptr<TCPClient>& client, 
			const Int32 entityID, 
			const std::u16string& username, 
			const std::reference_wrapper<QEventBus>& qBus, 
			const std::reference_wrapper<QEventBus>& qLogBus, 
			const std::reference_wrapper<ServerInterface>& serverInterface);

		virtual ~PlayerInterface();

		// virtual function goes here
		virtual bool onPlayerConnect(const std::shared_ptr<PlayerInterface>& player);
		virtual bool onPlayerDisconnect(const std::shared_ptr<PlayerInterface>& player);
		virtual bool onPlayerJoin(std::shared_ptr<World>& world);
		virtual bool onPlayerQuit(const std::shared_ptr<World>& world);

		std::shared_ptr<TCPClient>& getTCPClient();
		const std::reference_wrapper<ServerInterface>& getServerInterface();
		std::reference_wrapper<QEventBus>& getQEventBus();
		std::reference_wrapper<QEventBus>& getQEventLogBus();
		std::u16string getUsername() const;
	private:
		std::shared_ptr<TCPClient> client;
		Int32 entityID;
		std::u16string username;
		std::reference_wrapper<ServerInterface> serverInterface;
		std::reference_wrapper<QEventBus> qBus, qLogBus;
	};
}

