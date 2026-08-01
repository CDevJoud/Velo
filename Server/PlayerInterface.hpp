#pragma once
#include "TCPClient.hpp"
#include <memory>
#include "QEventBus.hpp"
#include "ServerInterface.hpp"

namespace velo {
	class PlayerInterface : public IntrusiveCounted{
	public:
		PlayerInterface(
			const Intrusive<TCPClient>& client, 
			const Int32 entityID, 
			const std::u16string& username, 
			const std::reference_wrapper<QEventBus>& qBus, 
			const std::reference_wrapper<QEventBus>& qLogBus, 
			const std::reference_wrapper<ServerInterface>& serverInterface);

		virtual ~PlayerInterface();

		// virtual function goes here
		virtual bool onPlayerConnect(const Intrusive<PlayerInterface>& player);
		virtual bool onPlayerDisconnect(const Intrusive<PlayerInterface>& player);
		virtual bool onPlayerJoin(Intrusive<World>& world);
		virtual bool onPlayerQuit(const Intrusive<World>& world);

		const Intrusive<TCPClient>& getTCPClient() const;
		const std::reference_wrapper<ServerInterface>& getServerInterface() const;
		const std::reference_wrapper<QEventBus>& getQEventBus() const;
		const std::reference_wrapper<QEventBus>& getQEventLogBus() const;
		std::u16string getUsername() const;
	private:
		Intrusive<TCPClient> client;
		Int32 entityID;
		std::u16string username;
		std::reference_wrapper<ServerInterface> serverInterface;
		std::reference_wrapper<QEventBus> qBus, qLogBus;
	};
}

