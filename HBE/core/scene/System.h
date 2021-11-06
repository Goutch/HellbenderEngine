#include "ECSScene.h"

namespace HBE {

	struct test {
		bool isTrue;
	};

	template<typename ... Ts>
	class System {
		virtual void init(Scene* scene) = 0;

		virtual void update() = 0;

		virtual void draw() = 0;

		virtual void render() = 0;
	};
}