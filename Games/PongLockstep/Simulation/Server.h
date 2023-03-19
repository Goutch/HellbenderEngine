
#pragma once

#include "Commands.h"

namespace PongLockstep {
	struct ServerInfo {
		int port;
		int max_clients;
		int max_channels;
	};


	class Server {
		ServerInfo info;
		ENetHost *enet_handle;
	protected:

		std::vector<ENetPeer *> clients;
		std::vector<uint32_t> free_client_ids;
	public:
		Server(const ServerInfo &info);

		void start();

		void pollEvents();

		void stop();

		bool send(const PacketInfo &packet_info);

	protected:
		virtual void onReceive(void *data, int client_id) = 0;

		virtual void onClientConnected(int client_id) = 0;
		virtual void onClientDisconnected(int client_id) = 0;
	};
}



