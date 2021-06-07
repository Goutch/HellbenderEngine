
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

    Shader::Shader(const std::vector<char> &source, SHADER_TYPE type) {
        instance = Graphics::getRenderer()->getResourceFactory()->createShader();
        this->type = type;
        setSource(source, type);
    }


    SHADER_TYPE Shader::getType() {
        return type;
    }

    void Shader::load(const std::string &path, SHADER_TYPE type) {
        std::vector<char> source;
        getSource(path, source);
        instance->setSource(source, type);
    }

    void Shader::setSource(const std::vector<char> &source, SHADER_TYPE type) {
        instance->setSource(source, type);
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

    const IShader *Shader::getInstance() const {
        return instance;
    }
}