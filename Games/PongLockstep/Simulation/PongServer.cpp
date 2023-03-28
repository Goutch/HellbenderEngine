#include "HBE.h"

using namespace HBE;

#include "PongServer.h"


namespace PongLockstep {

	void PongServer::onClientConnected(int client_id) {
		connected_clients++;
		if (connected_clients == N_PLAYERS && current_frame == 0) {
			for (int i = 0; i < STEP_OFFSET; i++) {

				StepData step_data{};
				step_data.frame_id = current_frame;
				step_data.delta = fix16(1) / STEP_PER_SECONDS;
				PacketInfo packet_info{};
				packet_info.channel = 0;
				packet_info.data = &step_data;
				packet_info.length = sizeof(StepData);
				packet_info.mode = PACKET_MODE_RELIABLE_ORDERED;
				send(packet_info);
			//	Log::message("Sent step data frame#" + std::to_string(current_frame));
				current_frame++;
			}
		}
	}

	void PongServer::onReceive(void *data, int client_id) {
		MESSAGE_TYPE type = *((MESSAGE_TYPE *) data);
		switch (type) {
			case MESSAGE_TYPE_STEP_INPUTS:
				StepInputsData player_input = *((StepInputsData *) data);
				inputs[client_id].push(player_input);
				int has_all_inputs = true;
				for (int player_index = 0; player_index < N_PLAYERS; player_index++) {
					if (inputs[player_index].empty() || inputs[player_index].front().frame_id != current_frame) {
						has_all_inputs = false;
						break;
					}
				}

				if (has_all_inputs) {

					StepData step_data{};
					step_data.frame_id = current_frame;
					step_data.inputs[0] = inputs[0].front();
					step_data.inputs[1] = inputs[1].front();
					step_data.delta = fix16(1) / STEP_PER_SECONDS;
					PacketInfo packet_info{};
					packet_info.channel = 0;
					packet_info.data = &step_data;
					packet_info.length = sizeof(StepData);
					packet_info.mode = PACKET_MODE_RELIABLE_ORDERED;
					send(packet_info);
					current_frame++;
					for (int player_index = 0; player_index < N_PLAYERS; player_index++) {
						inputs[player_index].pop();
					}
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