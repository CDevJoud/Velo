#include "LCEServer.hpp"
#include <thread>
#include "Packet.hpp"
#include <iostream>
#include "Player.hpp"

namespace velo {
	LCEServer::LCEServer() : qBus("LCEServer") {
		LCEServer::initQEventBusSubscriptions();
		LCEServer::world = std::make_shared<World>(std::ref(qBus), std::ref(*this));
	}
	LCEServer::~LCEServer() {
	
	}
	bool LCEServer::onClientConnect(const std::shared_ptr<TCPClient>& client, std::u16string& clientUsername) {
		
		Packet p;

		do {
			p.reset();
			if (client != nullptr) {
				if (client->receive(p) == Socket::Disconnected) {
					std::cout << "Client Disconnected!" << std::endl;
					return false;
				}
				if (p.getID() == Packet::ID::KeepAlive) {
					std::cout << "Client sent keep alive" << std::endl;
				}
			}
		} while (p.getID() == Packet::ID::KeepAlive);
		std::cout << "Client sent something else!" << std::endl;

		if (p.getID() == Packet::ID::Login) {
			std::cout << "Client sent Login Packet" << std::endl;
			LoginPacket lp{};
			p.parsePacket(&lp);
			std::cout << "Client Version: " << lp.clientVersion << " Username: ";
			for (const char16_t* p = lp.userName; *p; ++p) {
				std::cout << static_cast<char>(*p);
			}
			std::cout << std::endl;

			std::cout << "Sending Login Packet\n";
			client->send(
				Packet::createLogin(
					lp.clientVersion, lp.userName,
					u"default", lp.seed, 1,
					lp.dimension, lp.mapHeight, 32,
					lp.offlineXUID, lp.onlineXUID, lp.friendsOnlyUGC,
					lp.ugcPlayerVersion, lp.difficulty, lp.multiplayerInstanceID,
					lp.playerIndex, lp.playerSkinID, lp.playerCapeID, lp.isGuest,
					true, 129552, 320, 8
				));
			clientUsername = lp.userName;
			return true; //level up the connection!
		}
		return false;
	}
	bool LCEServer::onClientDisconnect(const std::shared_ptr<TCPClient>& client, std::u16string& clientUsername) {
		return false;
	}
	LCEServer::Config& LCEServer::getConfig() {
		return this->cfg;
	}
	Int32 LCEServer::runService() {
		qBus.runAsync();
		TCPServer::listen(this->cfg.port, this->cfg.ipAddress);
		ServerInterface::bIsServerRunning = true;
		while (ServerInterface::bIsServerRunning && !ServerInterface::bIsServerQuiting) {
			TCPClient client;
			auto status = TCPServer::accept(client);
			if (status != Socket::Error) {
				LCEServer::handleIncomingConnection(client);
			}
		}
		return 0;
	}
	std::shared_ptr<World>& LCEServer::getWorld() {
		return LCEServer::world;
	}
	void LCEServer::initQEventBusSubscriptions() {
		qBus.subscribe<event::client::Connect>([](const event::client::Connect& e) {
			std::u16string clientUsername;
			if (!e.server.get().onClientConnect(e.tcpClient, clientUsername)) {
				e.tcpClient->disconnect();
				return;
			}

			std::shared_ptr<Player> player = std::make_shared<Player>(
				e.tcpClient, 0, clientUsername, 
				e.server.get().qBus, e.server
			);

			e.server.get().insertPlayer(
				clientUsername,
				player
			);

			if (e.tcpClient->getNativeHandle() != Socket::Invalid) {
				e.server.get().qBus.post(event::player::Connect(
					player,
					clientUsername,
					e.server
				));
				
				std::thread(
					Player::handleConnection,
					std::dynamic_pointer_cast<Player>(
						e.server.get().getPlayer(clientUsername)
					)
				).detach();
			}

			});
		qBus.subscribe<event::client::Disconnect>([](const event::client::Disconnect& e) {
			std::u16string placeholder = u"";
			e.server.get().onClientDisconnect(e.tcpClient, placeholder);
			});
	}
	void LCEServer::handleIncomingConnection(TCPClient& client) {
		velo::Byte continue_ = 0;
		velo::Qword sent;
		client.send(&continue_, 1, sent);

		velo::Packet p;
		client.receive(p);
		if (p.getID() == Packet::ID::Invalid) {
			return;
		}
		PreLoginPacket plp{};
		p.parsePacket(&plp);
		client.send(Packet::createPreLogin(plp.lceNetworkVersion, plp.loginKey, 0, 0, 0, 0, nullptr, 0, 0, 0));
		qBus.post<event::client::Connect>(event::client::Connect(std::make_shared<TCPClient>(std::move(client)), std::ref<LCEServer>(*this)));
	}
}
