#pragma once

#include "fixed.h"
#include "enet/enet.h"

using namespace locknet;
namespace PongLockstep {
	enum PACKET_MODE {
		PACKET_MODE_UNRELIABLE_UNORDERED = ENET_PACKET_FLAG_UNSEQUENCED,
		PACKET_MODE_UNRELIABLE_ORDERED = 0,
		PACKET_MODE_RELIABLE_ORDERED = ENET_PACKET_FLAG_RELIABLE,
	};
	struct PacketInfo {
		int channel;
		int length;
		const void *data;
		PACKET_MODE mode;
	};

	enum MESSAGE_TYPE{
		MESSAGE_TYPE_CONNECTION_RESPONSE,
		MESSAGE_TYPE_STEP_INPUTS,
		MESSAGE_TYPE_STEP,
	};

	struct StepInputsData {
		MESSAGE_TYPE type = MESSAGE_TYPE_STEP_INPUTS;
		uint32_t frame_id;
		int paddle_direction = 0;
	};

	struct StepData {
		MESSAGE_TYPE type = MESSAGE_TYPE_STEP;
		uint32_t frame_id;
		fix16 delta;
		StepInputsData inputs[2];
	};


}

