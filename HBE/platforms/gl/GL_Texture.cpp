//
// Created by User on 04-Jun.-2020.
//

#include "GL_Texture.h"
#include "glad/glad.h"

namespace HBE {
    GL_Texture::GL_Texture() {
        glGenTextures(1, &texture_id);
    }

    void GL_Texture::setData(void *data, uint32_t width, uint32_t height, uint32_t depth, IMAGE_FORMAT format, IMAGE_DIMENSION dimension) {
        bind();
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        switch (texture_type) {
            case TEXTURE_FORMAT::R8: {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
                break;
            }
            case TEXTURE_FORMAT::RGBA8: {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
                break;
            }
            case TEXTURE_FORMAT::RGB8: {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                break;
            }
            case TEXTURE_FORMAT::DEPTH32: {
                glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
                             data);
                break;
            }
        }
        unbind();
    }

    void GL_Texture::bind(unsigned int slot) const {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void GL_Texture::unbind(unsigned int slot) const {
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, texture_id);
    }

    GL_Texture::~GL_Texture() {
        glDeleteTextures(1, &texture_id);
    }
}


