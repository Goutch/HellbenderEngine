#pragma once

#include "glm/glm.hpp"
#include "entt.hpp"
namespace HBE {
	typedef glm::mat2 mat2;
	typedef glm::mat3 mat3;
	typedef glm::mat4 mat4;


	typedef glm::vec2 vec2;
	typedef glm::vec3 vec3;
	typedef glm::vec4 vec4;

	typedef glm::vec<2,int32_t> vec2i;
	typedef glm::vec<3,int32_t> vec3i;
	typedef glm::vec<4,int32_t> vec4i;
	typedef glm::vec<2,uint32_t> vec2u;
	typedef glm::vec<3,uint32_t> vec3u;
	typedef glm::vec<4,uint32_t> vec4u;

	typedef glm::quat quat;

	typedef entt::entity EntityHandle;
}
