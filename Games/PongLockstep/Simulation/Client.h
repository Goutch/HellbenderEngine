#pragma once

#include "string"
#include "Commands.h"
#include "HBE/core/utility/Event.h"
using namespace HBE;
namespace PongLockstep {
	struct ClientInfo {
		int max_channels = 2;
		int server_port = 1234;
		std::string server_ip = "127.0.0.1";
	};

	class Client {
		ENetHost *enet_handle;
		ENetPeer *server_handle;
		ClientInfo info;
		uint32_t id;
		bool connected;
	public:
		Event<void *> on_receive_event;

		void pollEvents();

		int32_t getID() const;

		Client(const ClientInfo &info);

		~Client();

		bool connect(int timeout_ms);
		bool disconnect();
		bool send(const PacketInfo &packet_info);
	};

}

