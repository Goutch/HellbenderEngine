#include "Core.h"
#include "Configs.h"
#include "Shader.h"
#include <fstream>
#include <sstream>
#include "core/utility/Log.h"
#if RENDERER == OPENGL_RENDERER
#include <platforms/gl/GL_Shader.h>
#elif RENDERER == VULKAN_RENDERER

#endif

namespace HBE {

    Shader::Shader(SHADER_TYPE type, const std::string &source) {
        this->type = type;
    }

    Shader *Shader::create(SHADER_TYPE type, const std::string &source) {
#if RENDERER == OPENGL_RENDERER
        return new GL_Shader(type, source);
#elif RENDERER == VULKAN_RENDERER
        Log::error("Shaders are not implemented in vulkan")
        return nullptr;
#endif

    }

    std::string Shader::getSource(const std::string &path) {
        try {
            std::ifstream file;
            file.open(path);
            if (file.is_open()) {
                std::stringstream strStream;
                strStream << file.rdbuf();
                std::string str = strStream.str();
                file.close();
                return str;
            } else {
                Log::error("Unable to find file:" + path);
            }

        }
        catch (std::exception &e) {
            Log::error("failed to read file " + path + "\n" + e.what());
        }
        return "";

    }
}