#pragma once

#include "Core.h"
#include "string"
#include "Resource.h"
#include "map"
#include "memory"
#include "vector"

namespace HBE {
	enum SHADER_STAGE {
		SHADER_STAGE_NONE,
		SHADER_STAGE_COMPUTE,
		SHADER_STAGE_VERTEX,
		SHADER_STAGE_FRAGMENT,
		SHADER_STAGE_GEOMETRY,
		SHADER_STAGE_RAY_GEN,
		SHADER_STAGE_RAY_MISS,
		SHADER_STAGE_CLOSEST_HIT,
		SHADER_STAGE_ANY_HIT,
		SHADER_STAGE_INTERSECTION,
	};


	struct ShaderInfo {
		SHADER_STAGE stage = SHADER_STAGE_NONE;
		std::string path = "";
		std::string preamble = "";
	};

	class HB_API Shader : public Resource {
	protected:
		vec3i workgroup_size;
	public:
		virtual SHADER_STAGE getStage() const = 0;

		virtual ~Shader() = default;

		static void getSource(const std::string &path, std::string &buffer);

		vec3i getWorkgroupSize();
	};
}

