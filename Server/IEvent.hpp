#pragma once

#include <memory>
#include <functional>
#include <optional>
#include <source_location>
#include "Intrusive.hpp"
#define IEVENT_DECLARE_EVENT_NAME(_name) virtual const char const* name() const override { return "velo.event." _name; }

#ifdef _WIN32
#ifdef ERROR
#undef ERROR
#endif
#endif
namespace velo {
	class LCEServer;
	class Player;
	class World;
	class PlayerInterface;
	struct IEvent {
		virtual ~IEvent() {}
		[[nodiscard]] virtual const char const* name() const = 0;
	};

	namespace event {

		struct Log : IEvent {
			IEVENT_DECLARE_EVENT_NAME("log")
			enum Severity {
				INFO = 0x01,
				WARN = 0x06,
				ERROR = 0x04,
				DEBUG = 0x08,
				FATAL = 0x4F
			};
			Log(
				Severity s = Severity::INFO,
				const std::string m = "",
				const std::string& channel = "default",
				std::optional<std::source_location> loc = std::nullopt) : severity(s), msg(m), channel(channel), location(loc) {}
			Severity severity;
			std::string msg;
			std::string channel = "default";
			std::optional<std::source_location> location;
		};

		namespace client {
			/*struct Connect : public IEvent {
				IEVENT_DECLARE_EVENT_NAME("client.connect")
				Connect(
					const Intrusive<TCPClient>& client,
					const std::reference_wrapper<LCEServer>& server
				) : tcpClient(client), server(server) {}
				Intrusive<TCPClient> tcpClient;
				std::reference_wrapper<LCEServer> server;
			};
			struct Disconnect : public IEvent {
				IEVENT_DECLARE_EVENT_NAME("client.disconnect")
				Disconnect(
					const Intrusive<TCPClient>& client, 
					const std::reference_wrapper<LCEServer>& server
				) : tcpClient(client), server(server) {}
				Intrusive<TCPClient> tcpClient;
				std::reference_wrapper<LCEServer> server;
			};*/
		}
		namespace player {
			//struct Connect : public IEvent {
			//	IEVENT_DECLARE_EVENT_NAME("player.connect")
			//	Connect(
			//		const Intrusive<PlayerInterface>& player, 
			//		const std::u16string& username, 
			//		const std::reference_wrapper<LCEServer>& server
			//	) : player(player), username(username), server(server) {}
			//	Intrusive<PlayerInterface> player;
			//	std::reference_wrapper<LCEServer> server;
			//	std::u16string username;
			//};
			//struct Disconnect : public IEvent {
			//	IEVENT_DECLARE_EVENT_NAME("player.disconnect")
			//	Disconnect(
			//		const Intrusive<PlayerInterface>& player,
			//		const std::u16string& username, 
			//		const std::reference_wrapper<LCEServer> server
			//	) : player(player), username(username), server(server) {}
			//	Intrusive<PlayerInterface> player;
			//	std::u16string username;
			//	std::reference_wrapper<LCEServer> server;
			//};

			//struct Join : public IEvent {
			//	IEVENT_DECLARE_EVENT_NAME("player.join")
			//	Join(
			//		const Intrusive<PlayerInterface>& player,
			//		const Intrusive<World>& world,
			//		const std::reference_wrapper<LCEServer> server
			//	) : player(player), world(world), server(server) {}

			//	Intrusive<PlayerInterface> player;
			//	Intrusive<World> world;
			//	std::reference_wrapper<LCEServer> server;
			//};

			//struct Quit : public IEvent {
			//	IEVENT_DECLARE_EVENT_NAME("player.quit")
			//	Quit(
			//		const Intrusive<PlayerInterface>& player,
			//		const Intrusive<World>& world,
			//		const std::reference_wrapper<LCEServer> server
			//		//const std::function<void()> denied_request_callback = nullptr
			//	) : player(player), world(world), server(server) /*deniedReqCB(denied_request_callback)*/{}

			//	Intrusive<PlayerInterface> player;
			//	Intrusive<World> world;
			//	std::reference_wrapper<LCEServer> server;
			//	//std::function<void()> deniedReqCB;
			//};
		}
	}
}

