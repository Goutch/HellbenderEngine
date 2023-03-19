#include "HBE/core/utility/Log.h"
#include "Client.h"
#include "enet/enet.h"


using namespace HBE;
namespace PongLockstep {
	Client::Client(const ClientInfo &info) {
		this->info = info;
		if (enet_initialize() != 0) {
			Log::error("An error occurred while initializing ENet.");
		}
	}

	Client::~Client() {
		disconnect();
		atexit(enet_deinitialize);
	}

	bool Client::disconnect() {
		if (connected) {
			enet_peer_disconnect(server_handle, 0);
			enet_host_destroy(enet_handle);
		}
		return true;
	}

	bool Client::connect(int timeout_ms) {
		connected = false;
		enet_handle = enet_host_create(NULL /* create a client host */,
									   1 /* only allow 1 outgoing connection */,
									   info.max_channels /* allow up 2 channels to be used, 0 and 1 */,
									   0 /* assume any amount of incoming bandwidth */,
									   0 /* assume any amount of outgoing bandwidth */);
		if (enet_handle == NULL) {
			Log::error("An error occurred while trying to create an ENet client host.\n");
			return connected;
		}

		ENetAddress address;
		ENetEvent event;
		enet_address_set_host(&address, info.server_ip.c_str());
		address.port = info.server_port;
		server_handle = enet_host_connect(enet_handle, &address, info.max_channels, 0);

		if (server_handle == NULL) {
			fprintf(stderr,
					"No available peers for initiating an ENet connection.\n");
			return connected;
		}
		if (enet_host_service(enet_handle, &event, timeout_ms) > 0 &&
			event.type == ENET_EVENT_TYPE_CONNECT) {
			printf("Connection to %s:%i succeeded.\n", info.server_ip.c_str(), info.server_port);

		} else {
			/* Either the 5 seconds are up or a disconnect event was */
			/* received. Reset the peer in the event the 5 seconds   */
			/* had run out without any significant event.            */
			enet_peer_reset(server_handle);
			Log::error("Connection to " + info.server_ip + " failed.");
			return connected;
		}
		if (enet_host_service(enet_handle, &event, timeout_ms) > 0) {

			switch (event.type) {
				case ENET_EVENT_TYPE_RECEIVE:
					id = *(uint32_t *) event.packet->data;
					printf("Connected as Client# %i\n", id);
					enet_packet_destroy(event.packet);
					connected = true;
					break;

				case ENET_EVENT_TYPE_DISCONNECT:
					printf("%s disconnected.\n", (char *) event.peer->data);
					/* Reset the peer's client information. */
					event.peer->data = NULL;
			}
		} else {
			return connected;
		}

		return connected;
	}

	std::string ipToString(int ip) {
		std::string ip_str;
		ip_str += std::to_string(ip & 0xFF);
		ip_str += ".";
		ip_str += std::to_string((ip >> 8) & 0xFF);
		ip_str += ".";
		ip_str += std::to_string((ip >> 16) & 0xFF);
		ip_str += ".";
		ip_str += std::to_string((ip >> 24) & 0xFF);
		return ip_str;
	}

	void Client::pollEvents() {
		ENetEvent event;
		while (enet_host_service(enet_handle, &event, 0) > 0) {
			switch (event.type) {
				case ENET_EVENT_TYPE_CONNECT:
					Log::error("error: received a connection event from " + ipToString(event.peer->address.host) + std::to_string(event.peer->address.port));
					break;
				case ENET_EVENT_TYPE_RECEIVE:
					on_receive_event.invoke(event.packet->data);
					enet_packet_destroy(event.packet);
					break;

				case ENET_EVENT_TYPE_DISCONNECT:
					Log::error(std::string((char *) event.peer->data) + "%s disconnected.");
					/* Reset the peer's client information. */
					event.peer->data = NULL;
			}
		}
	}

	bool Client::send(const PacketInfo &packet_info) {
		ENetPacket *packet = enet_packet_create(packet_info.data, packet_info.length, packet_info.mode);
		if (enet_peer_send(server_handle, packet_info.channel, packet) < 0) {
			printf("error: failed to send packet to server.\n");
			enet_packet_destroy(packet);
			return false;
		}
		return true;
	}

	int32_t Client::getID() const {
		return id;
	}


}

