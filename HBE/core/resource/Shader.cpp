
#include "Shader.h"
#include <fstream>

#include "core/graphics/Graphics.h"
#include "core/utility/Log.h"
#include "core/graphics/Renderer.h"
#ifdef OPENGL_RENDERER
#include <platforms/gl/GL_Shader.h>
#else
#ifdef VULKAN_RENDERER
#include <platforms/vk/VK_Shader.h>
#include "platforms/vk/VK_Renderer.h"
#include "platforms/vk/VK_Device.h"
#endif
#endif
namespace HBE {

    Shader::Shader(SHADER_TYPE type) {
        this->type = type;
    }


    Shader *Shader::create(SHADER_TYPE type, const std::string &source) {
#ifdef OPENGL_RENDERER
        return new GL_Shader(type, source);
#else
#ifdef VULKAN_RENDERER
        VK_Renderer* renderer=static_cast<VK_Renderer*>(Graphics::getRenderer());
        return new VK_Shader(renderer->getDevice().getHandle(),type, source);
#endif
#endif
        return nullptr;
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

    SHADER_TYPE Shader::getType() {
        return type;
    }
}