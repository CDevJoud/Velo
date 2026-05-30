#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#pragma warning(disable: 4996)
#pragma comment(lib, "ws2_32.lib")

#include "TCPServer.hpp"
#include "Packet.hpp"

#include <bit>

int main() {
	WSADATA data;
	if (WSAStartup(MAKEWORD(2, 2), &data) != 0)
		return 1;

	velo::TCPServer server;
	server.listen();
	while (true) {
		velo::TCPClient client;
		server.accept(client);

		std::cout << "New client connected!" << std::endl;

		velo::Byte continue_ = 0;
		velo::Qword sent;
		client.send(&continue_, 1, sent);

		velo::Packet p;
		client.receive(p);

		auto pID = p.getID();
		if (pID == velo::Packet::ID::PreLogin) {
			velo::PreLoginPacket plp{};
			p.parsePacket(&plp);
		}
		client.disconnect();
	}

	server.stop();

	WSACleanup();
}