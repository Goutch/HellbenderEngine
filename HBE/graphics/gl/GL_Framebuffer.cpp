//
// Created by User on 13-Jun.-2020.
//

#include "GL_Framebuffer.h"
#include "glad/glad.h"
#include "core/utility/Log.h"

GL_Framebuffer::GL_Framebuffer() {
    glGenFramebuffers(1, &fbo);
    glGenRenderbuffers(1, &rbo);
    glGenTextures(1,&texture);
}
GL_Framebuffer::GL_Framebuffer(int width, int height) {
    glGenFramebuffers(1, &fbo);
    glGenRenderbuffers(1, &rbo);
    glGenTextures(1,&texture);
    setResolution(width,height);
}

void GL_Framebuffer::bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

void GL_Framebuffer::unbind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GL_Framebuffer::~GL_Framebuffer() {
    glDeleteFramebuffers(1, &fbo);
    glDeleteRenderbuffers(1,&rbo);
    glDeleteTextures(1,&texture);
}

void GL_Framebuffer::setResolution(int width,int height) {
    this->width=width;
    this->height=height;
    bind();
    bindTexture();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D, texture, 0);


    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,width, height);

    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if(fboStatus != GL_FRAMEBUFFER_COMPLETE)
        Log::error("Framebuffer is not complete!");
    unbindTexture();
    unbind();
}

void GL_Framebuffer::unbindTexture() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}

void GL_Framebuffer::bindTexture() const {
    glBindTexture(GL_TEXTURE_2D, texture);
}





