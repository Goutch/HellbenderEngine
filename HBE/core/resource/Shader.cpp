
#include "Shader.h"
#include <fstream>
#include "sstream"
#include "core/graphics/Renderer.h"
#include "core/resource/ResourceFactory.h"

namespace HBE {

    SHADER_STAGE Shader::getType() {
        return type;
    }

    void Shader::load(const std::string &path, SHADER_STAGE type) {
        std::vector<char> source;
        getSource(path, source);
        setSource(source.data(),type);
    }

    void Shader::getSource(const std::string &path, std::vector<char> &buffer) {
        try {
            std::ifstream file;
            file.open(path,std::ios::ate);
            if (file.is_open()) {
                size_t size=(size_t) file.tellg();
                buffer.resize(size);
                file.seekg(0);
                file.read(buffer.data(), size);
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