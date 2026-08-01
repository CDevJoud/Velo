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
		virtual bool onClientConnect(Intrusive<TCPClient>& client, std::u16string& clientUsername);
		virtual bool onClientDisconnect(Intrusive<TCPClient>& client, std::u16string& clientUsername);
	public:
		bool isServerRunning() const;

		bool isServerQuiting() const;

		Intrusive<PlayerInterface> getPlayer(const std::u16string& username);

		bool insertPlayer(const std::u16string& username, const Intrusive<PlayerInterface>& player);

		bool removePlayer(const std::u16string& username);
	private:
		friend class LCEServer;
		std::unordered_map<std::u16string, Intrusive<PlayerInterface>> players;
		bool bIsServerRunning = false;
		bool bIsServerQuiting = false;
	};
}