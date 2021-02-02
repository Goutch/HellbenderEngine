
#include "ShaderProgram.h"
#include "Shader.h"
#include <platforms/gl/GL_ShaderProgram.h>
#include "core/serialization/Serializer.h"
namespace HBE {
    ShaderProgram *ShaderProgram::create() {
#ifdef OPENGL_RENDERER
        return new GL_ShaderProgram();
#else
#ifdef VULKAN_RENDERER

#endif
#endif
        return nullptr;
    }

    void ShaderProgram::serialize(Serializer *serializer) const {
        serializer->begin("ShaderProgram");
        for (auto s:shaders) {
            switch (s.first) {
                case SHADER_TYPE::VERTEX:
                    serializer->addField("vertex_path", s.second);
                    break;
                case SHADER_TYPE::FRAGMENT:
                    serializer->addField("fragment_path", s.second);
                    break;
                case SHADER_TYPE::COMPUTE:

                    break;
                case SHADER_TYPE::GEOMETRY:
                    break;
            }
        }
        serializer->end();
    }

    void ShaderProgram::deserialize(Deserializer *deserializer) {

    }


}