#pragma once
#include "TCPClient.hpp"
#include <memory>
#include <unordered_map>

namespace velo {
	class PlayerInterface;

	class ServerInterface {
	public:
		ServerInterface() = default;
		ServerInterface(bool isServerRunning);
		virtual bool onClientConnect(const std::shared_ptr<TCPClient>& client, std::u16string& clientUsername);
		virtual bool onClientDisconnect(const std::shared_ptr<TCPClient>& client, std::u16string& clientUsername);
	public:
		bool isServerRunning() const;

		bool isServerQuiting() const;

		std::shared_ptr<PlayerInterface> getPlayer(const std::u16string& username);

		bool insertPlayer(const std::u16string& username, const std::shared_ptr<PlayerInterface>& player);

		bool removePlayer(const std::u16string& username);
	private:
		friend class LCEServer;
		std::unordered_map<std::u16string, std::weak_ptr<PlayerInterface>> players;
		bool bIsServerRunning = false;
		bool bIsServerQuiting = false;
	};
}