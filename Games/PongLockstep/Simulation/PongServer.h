#pragma once

#include <queue>
#include "Server.h"

namespace PongLockstep {
	class PongServer : public Server {
		const int N_PLAYERS=2;
		int connected_clients = 0;
		struct Step {
			int step;
			StepData data;
		};

		uint32_t current_frame = 0;
		std::vector<StepInputsData> inputs[2];
	public:
		PongServer(const ServerInfo &info);

	private:
		void onClientConnected(int client_id) override;

		void onClientDisconnected(int client_id) override;

		void onReceive(void *data, int client_id) override;


	};
}

