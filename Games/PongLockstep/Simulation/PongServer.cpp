#include "HBE.h"

using namespace HBE;

#include "PongServer.h"


namespace PongLockstep {

	void PongServer::onClientConnected(int client_id) {
		connected_clients++;
		if (connected_clients == N_PLAYERS && current_frame == 0) {
			StepData step_data{};
			step_data.frame_id = 0;
			step_data.delta = fix16(1) / 144;
			PacketInfo packet_info{};
			packet_info.channel = 0;
			packet_info.data = &step_data;
			packet_info.length = sizeof(StepData);
			packet_info.mode = PACKET_MODE_RELIABLE_ORDERED;
			send(packet_info);
			Log::message("Sent step data frame 0");
		}
	}

	void PongServer::onReceive(void *data, int client_id) {
		MESSAGE_TYPE type = *((MESSAGE_TYPE *) data);
		switch (type) {
			case MESSAGE_TYPE_STEP_INPUTS:
				StepInputsData player_input = *((StepInputsData *) data);
				inputs[client_id].push_back(player_input);
				bool has_all_inputs = true;
				for (int player_index = 0; player_index< N_PLAYERS; player_index++) {
					if (inputs[player_index].size() <= current_frame) {
						has_all_inputs = false;
						break;
					}
				}
				if (has_all_inputs) {
					StepData step_data{};
					step_data.frame_id = current_frame;
					step_data.inputs[0] = inputs[0].front();
					step_data.inputs[1] = inputs[1].front();
					step_data.delta = fix16(1) / 144;
					PacketInfo packet_info{};
					packet_info.channel = 0;
					packet_info.data = &step_data;
					packet_info.length = sizeof(StepData);
					packet_info.mode = PACKET_MODE_RELIABLE_ORDERED;
					send(packet_info);
					current_frame++;
				}
				break;
		}
	}

	PongServer::PongServer(const ServerInfo &info) : Server(info) {

	}

	void PongServer::onClientDisconnected(int client_id) {
		connected_clients--;
	}
}