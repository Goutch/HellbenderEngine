
#include "ShaderProgram.h"
#include <platforms/gl/GL_ShaderProgram.h>
#include "core/serialization/Serializer.h"
ShaderProgram *ShaderProgram::create() {
#if RENDERER == OPENGL_RENDERER
    return new GL_ShaderProgram();
#elif RENDERER == VULKAN_RENDERER

#endif
    return nullptr;
}

void ShaderProgram::serialize(Serializer *serializer) const {
    serializer->begin("ShaderProgram");
    for (auto s:shaders) {
        switch (s.first) {
            case SHADER_TYPE::VERTEX:
                serializer->addField("vertex_path",s.second);
                break;
            case SHADER_TYPE::FRAGMENT:
                serializer->addField("fragment_path",s.second);
                break;
            case SHADER_TYPE::COMPUTE:

                break;
        }
    }
    serializer->end();
}

void ShaderProgram::deserialize(Deserializer *deserializer) {

}
