
#include <string>
#include "Server.h"
#include "stdio.h"

namespace PongLockstep {
	Server::Server(const ServerInfo &info) {
		this->info = info;
		free_client_ids.reserve(info.max_clients);
		for (int i = 0; i < info.max_clients; i++) {
			free_client_ids.push_back((info.max_clients - 1) - i);
		}
		clients.resize(info.max_clients, nullptr);
	}

	void Server::start() {
		if (enet_initialize() != 0) {
			fprintf(stderr, "An error occurred while initializing ENet.\n");
		}
		ENetAddress address;
		address.host = ENET_HOST_ANY;
		address.port = info.port;
		enet_handle = enet_host_create(&address /* the address to bind the server host to */,
									   info.max_clients     /* allow up to 32 clients and/or outgoing connections */,
									   info.max_channels      /* allow up to 2 channels to be used, 0 and 1 */,
									   0      /* assume any amount of incoming bandwidth */,
									   0      /* assume any amount of outgoing bandwidth */);
		if (enet_handle == NULL) {
			fprintf(stderr, "An error occurred while trying to create an ENet server host.");
		}

		pollEvents();
	}

	int i = 0;
	std::string frame_counter = std::to_string(i);

	void Server::pollEvents() {
		ENetEvent event;
		i++;
		frame_counter = std::to_string(i);


		while (enet_host_service(enet_handle, &event, 10) > 0) {

			switch (event.type) {
				case ENET_EVENT_TYPE_CONNECT: {
					uint32_t id = free_client_ids.back();
					printf("Client#%u connected from  %d.%d.%d.%d:%u\n",
						   id,
						   event.peer->address.host & 0xFF,
						   (event.peer->address.host >> 8) & 0xFF,
						   (event.peer->address.host >> 16) & 0xFF,
						   (event.peer->address.host >> 24) & 0xFF,
						   (uint32_t) event.peer->address.port);
					PacketInfo packet_info{};
					packet_info.mode = PACKET_MODE_RELIABLE_ORDERED;
					packet_info.channel = 0;
					packet_info.data = &id;
					packet_info.length = sizeof(uint32_t);
					send(packet_info);
					clients[id] = event.peer;
					event.peer->data = new uint32_t(id);
					free_client_ids.pop_back();
					onClientConnected(id);
				}
					break;
				case ENET_EVENT_TYPE_RECEIVE:
					onReceive(event.packet->data, *(uint32_t *) event.peer->data);
					enet_packet_destroy(event.packet);
					break;

				case ENET_EVENT_TYPE_DISCONNECT:
					printf("Client#%i disconnected.\n", event.peer->connectID);
					uint32_t *id_ptr = (uint32_t *) event.peer->data;
					clients[*id_ptr] = nullptr;
					free_client_ids.push_back(*id_ptr);
					delete id_ptr;
			}
		}

	}

	void Server::stop() {
		enet_host_destroy(enet_handle);
		enet_deinitialize();
	}

	bool Server::send(const PacketInfo &packet_info) {
		for (int i = 0; i < enet_handle->connectedPeers; ++i) {
			ENetPacket *packet = enet_packet_create(packet_info.data, packet_info.length, packet_info.mode);
			if (enet_peer_send(&enet_handle->peers[i], packet_info.channel, packet) < 0) {
				printf("error: failed to send packet to client[%i].\n", i);
				enet_packet_destroy(packet);
				return false;
			}
		}

		return true;
	}
}

