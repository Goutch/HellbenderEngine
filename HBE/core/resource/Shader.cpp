
#include "Shader.h"
#include <fstream>
#include "sstream"
#include "core/graphics/Graphics.h"
#include "core/utility/Log.h"
#include "core/graphics/Renderer.h"
#include "core/resource/IResourceFactory.h"

namespace HBE {

    Shader::~Shader() {
        delete instance;
    }

    Shader::Shader() {
        instance = Graphics::getRenderer()->getResourceFactory()->createShader();
    }

    Shader::Shader(const std::string &source, SHADER_TYPE type) {
        instance = Graphics::getRenderer()->getResourceFactory()->createShader();
        this->type = type;
        setSource(source, type);
    }


    SHADER_TYPE Shader::getType() {
        return type;
    }

    void Shader::load(const std::string &path, SHADER_TYPE type) {
        std::string source;
        getSource(path, source);
        instance->setSource(source, type);
    }

    void Shader::setSource(const std::string &source, SHADER_TYPE type) {
        instance->setSource(source, type);
    }

    void Shader::getSource(const std::string &path, std::string &buffer) {
        try {
            std::ifstream file;
            file.open(path);
            if (file.is_open()) {
                std::stringstream strStream;
                strStream << file.rdbuf();
                buffer = strStream.str();
                file.close();
            } else {
                Log::error("Unable to find file:" + path);
            }

        }
        catch (std::exception &e) {
            Log::error("failed to read file " + path + "\n" + e.what());
        }
    }

    const void *Shader::getHandle() const {
        return instance->getHandle();
    }

}