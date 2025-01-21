#pragma once

#include "vector"
#include "string"
#include "core/resource/Shader.h"

namespace HBE {
	class ShaderCompiler {
	public:
		static void GLSLToSpirV(const char *source,
		                        size_t size,
		                        std::vector<uint32_t> &spirv,
		                        SHADER_STAGE type,
		                        const std::string &string,
		                        const std::string &entry_point);
	};

}
