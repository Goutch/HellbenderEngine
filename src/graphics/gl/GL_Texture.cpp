//
// Created by User on 04-Jun.-2020.
//

#include "GL_Texture.h"
#include "glad/glad.h"

GL_Texture::GL_Texture() {
    glGenTextures(1, &texture_id);
}

void GL_Texture::setData(unsigned char *data, int width, int height, Texture::TEXTURE_TYPE texture_type) {
    bind();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    switch (texture_type) {
        case TEXTURE_TYPE::R8: {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);
            break;
        }
        case TEXTURE_TYPE::RGBA8: {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            break;
        }
        case TEXTURE_TYPE::RGB8: {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            break;
        }
        case TEXTURE_TYPE::DEPTH32: {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, data);
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


unsigned int GL_Texture::getId() {
    return texture_id;
}

GL_Texture::~GL_Texture() {
    glDeleteTextures(1, &texture_id);
}


