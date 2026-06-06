#include "LCEServer.hpp"
#include <thread>
#include "Packet.hpp"
#include <iostream>
#include "Player.hpp"
#include "Logger.hpp"

#define LOG_INFO(msg)  qLogBus.post(event::Log(event::Log::Severity::INFO, msg));
#define LOG_DEBUG(msg) qLogBus.post(event::Log(event::Log::Severity::DEBUG, msg));
#define LOG_WARN(msg)  qLogBus.post(event::Log(event::Log::Severity::WARN, msg));
#define LOG_ERROR(msg) qLogBus.post(event::Log(event::Log::Severity::ERROR, msg));
#define LOG_FATAL(msg) qLogBus.post(event::Log(event::Log::Severity::FATAL, msg));
#define LOG_INFO_TRACE(msg)  qLogBus.post(event::Log(event::Log::Severity::INFO, msg, "default", std::source_location::current()));
#define LOG_DEBUG_TRACE(msg) qLogBus.post(event::Log(event::Log::Severity::DEBUG, msg, "default", std::source_location::current()));
#define LOG_WARN_TRACE(msg)  qLogBus.post(event::Log(event::Log::Severity::WARN, msg, "default", std::source_location::current()));
#define LOG_ERROR_TRACE(msg) qLogBus.post(event::Log(event::Log::Severity::ERROR, msg, "default", std::source_location::current()));
#define LOG_FATAL_TRACE(msg) qLogBus.post(event::Log(event::Log::Severity::FATAL, msg, "default", std::source_location::current()));

namespace velo {
	LCEServer::LCEServer() : qBus("LCEServer"), qLogBus("LCEServerLog") {
		qBus.runAsync();
		qLogBus.runAsync();
		LCEServer::logger = std::make_unique<Logger>(std::ref(qLogBus));
		LCEServer::initQEventBusSubscriptions();

		LOG_INFO("Starting the server please wait...");
		std::this_thread::sleep_for(std::chrono::milliseconds(5000));

		LOG_INFO("Loading default world");
		LCEServer::world = std::make_shared<World>(std::ref(qBus), std::ref(qLogBus), std::ref(*this));
		LOG_INFO("Done.");
	}
	LCEServer::~LCEServer() {

	}
	bool LCEServer::onClientConnect(const std::shared_ptr<TCPClient>& client, std::u16string& clientUsername) {
		
		Packet p;
		std::string clientName = client->getRemoteAddress() + ":" + std::to_string(client->getRemotePort());
		do {
			p.reset();
			if (client != nullptr) {
				if (client->receive(p) == Socket::Disconnected) {
					LOG_INFO("@velo: Client " + clientName + " disconnected.");
					return false;
				}
				if (p.getID() == Packet::ID::KeepAlive) {
					LOG_DEBUG("@velo: Client " + clientName + " sent keep alive.");
				}
			}
		} while (p.getID() == Packet::ID::KeepAlive);
		LOG_DEBUG("@velo: Client " + clientName + " sent something else.");

		if (p.getID() == Packet::ID::Login) {
			LOG_DEBUG("@velo: Client " + clientName + " sent something login packet.");

			LoginPacket lp{};
			if (!p.parsePacket(&lp)) {
				LOG_DEBUG("@velo: Client " + clientName + " is a bad client!");
				client->disconnect();
				return false;
			}
			clientUsername = lp.userName;
			std::string displayName(clientUsername.begin(), clientUsername.end());
			LOG_INFO("@velo: Client " + clientName + " version: " + std::to_string(lp.clientVersion) + " username: " +  displayName);
			

			LOG_DEBUG("@velo: Sending login packet to " + clientName + " known as " + displayName);
			auto res = client->send(
				Packet::createLogin(
					lp.clientVersion, lp.userName,
					u"default", lp.seed, 1,
					lp.dimension, lp.mapHeight, 32,
					lp.offlineXUID, lp.onlineXUID, lp.friendsOnlyUGC,
					lp.ugcPlayerVersion, lp.difficulty, lp.multiplayerInstanceID,
					lp.playerIndex, lp.playerSkinID, lp.playerCapeID, lp.isGuest,
					true, 129552, 320, 8
				));
			if (res == Socket::Error || res == Socket::Disconnected) {
				LOG_ERROR("Failed to send login packet to " + clientName + " known as " + displayName + ".\t\n Client disconnected!");
				return false;
			}
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
		if (!TCPServer::listen(this->cfg.port, this->cfg.ipAddress)) {
			LOG_FATAL(std::format("could not bind to port! {}:{}", cfg.port, cfg.ipAddress));
			return -1;
		}
		ServerInterface::bIsServerRunning = true;
		while (ServerInterface::bIsServerRunning && !ServerInterface::bIsServerQuiting) {
			TCPClient client(std::ref(qBus));
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
			// for logging!
			QEventBus& qLogBus = e.server.get().qLogBus;

			std::shared_ptr<Player> player = std::make_shared<Player>(
				e.tcpClient, 0, clientUsername, 
				e.server.get().qBus, e.server.get().qLogBus, e.server
			);

			std::string clientUsername8(clientUsername.begin(), clientUsername.end());
			LOG_INFO("@velo: created a new player named " + clientUsername8);

			e.server.get().insertPlayer(
				clientUsername,
				player
			);

			LOG_INFO("@velo: inserted the player " + clientUsername8 + " to server registry!");

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
			else {
				LOG_ERROR("@velo: for some reason client socket became invalid! removing " + clientUsername8 + " from server registry");
				if (!e.server.get().removePlayer(clientUsername)) {
					LOG_DEBUG("@velo: could not remove " + clientUsername8 + " from server registry");
				}
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
		auto res = client.receive(p);
		if (res == Socket::Error || res == Socket::Disconnected) {
			return;
		}
		PreLoginPacket plp{};
		if (!p.parsePacket(&plp)) {
			// bad client!
			LOG_INFO("@velo: Client " + client.getRemoteAddress() + ":" + std::to_string(client.getRemotePort()) + " is not a valid LCE client! Closing connection.")
			return;
		}
		res = client.send(Packet::createPreLogin(plp.lceNetworkVersion, plp.loginKey, 0, 0, 0, 0, nullptr, 0, 0, 0));
		if (res == Socket::Error || res == Socket::Disconnected) {
			return;
		}
		LOG_INFO("@velo: New client connected!");
		qBus.post<event::client::Connect>(event::client::Connect(std::make_shared<TCPClient>(std::move(client)), std::ref<LCEServer>(*this)));
	}
}
