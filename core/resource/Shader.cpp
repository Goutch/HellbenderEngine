
#include "Shader.h"
#include "core/resource/ResourceFactory.h"
#include "core/utility/Log.h"
#include "fstream"

namespace HBE {

	void Shader::getSource(const std::string &path, std::string &buffer) {
		try {
			std::string res_path = RESOURCE_PATH + path;
			std::ifstream file;
			file.open(res_path, std::ios::ate);
			if (file.is_open()) {
				size_t size = (size_t) file.tellg();
				buffer.resize(size);
				file.seekg(0);
				file.read(buffer.data(), size);
				file.close();
			} else {
				Log::error("Unable to find file:" + path);
			}
			/*
			FILE *file = fopen(res_path.c_str(), "r");
			if (ferror(file)) {
				Log::error("Unable to find file:" + path);
			}
			while (!feof(file)) {
				fgetc(file);
				size++;
			}
			rewind(file);
			*buffer = new char[size + 1];
			for (int i = 0; i < size; ++i) {
				char c = getc(file);
				(*buffer)[i] = c;
			}
			buffer[size] = 0;
			fclose(file);*/
		}
		catch (std::exception &e) {
			Log::error("failed to read file " + path + "\n" + e.what());
		}
	}

	vec3i Shader::getWorkgroupSize() {
		return workgroup_size;
	}
}