#pragma once

#include "Core.h"
#include <cstdlib>

namespace HBE {
	class HB_API Random {
	public:
		static float floatRange(float min, float max);

		static int intRange(int min, int max);
	};
}
