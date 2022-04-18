
#include "Shader.h"
#include <fstream>
#include "sstream"
#include "core/graphics/Renderer.h"
#include "core/resource/ResourceFactory.h"

namespace HBE {

	void Shader::getSource(const std::string &path, char **buffer, size_t &size) {
		try {
			std::ifstream file;
			std::string res_path = RESOURCE_PATH + path;
			file.open(res_path, std::ios::ate);
			if (file.is_open()) {
				size = (size_t) file.tellg();
				*buffer = (char *) malloc(size * sizeof(char));

				file.seekg(0);
				file.read(*buffer, size);

				file.close();
			} else {
				Log::error("Unable to find file:" + path);
			}

		}
		catch (std::exception &e) {
			Log::error("failed to read file " + path + "\n" + e.what());
		}
	}
}