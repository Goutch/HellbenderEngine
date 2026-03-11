#pragma once

#include "vector"
#include "string"

namespace HBE {
	class ShaderCompiler {
	public:
		static void getSource(const std::string &path, std::string &buffer);
		static void GLSLToSpirV(const char *source,
		                        size_t size,
		                        std::vector<uint32_t> &spirv,
		                        SHADER_STAGE type,
		                        const std::string &shader_path,
		                        const std::string &preamble = "");
	};

}
