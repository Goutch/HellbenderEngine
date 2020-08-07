//
// Created by User on 14-Jun.-2020.
//

#include "GL_MultisamplingFramebuffer.h"
#include "glad/glad.h"
#include <core/utility/Log.h>

GL_MultisamplingFramebuffer::GL_MultisamplingFramebuffer() {
    glGenFramebuffers(1, &ms_fbo);
    glGenFramebuffers(1, &fbo);
    glGenRenderbuffers(1, &rbo);
    glGenTextures(1, &texture);
    glGenTextures(1, &ms_texture);
}

GL_MultisamplingFramebuffer::GL_MultisamplingFramebuffer(int width, int height) {
    glGenFramebuffers(1, &ms_fbo);
    glGenFramebuffers(1, &fbo);
    glGenRenderbuffers(1, &rbo);
    glGenTextures(1, &texture);
    glGenTextures(1, &ms_texture);
    setResolution(width, height);
}

GL_MultisamplingFramebuffer::~GL_MultisamplingFramebuffer() {
    glDeleteFramebuffers(1, &ms_fbo);
    glDeleteFramebuffers(1, &fbo);
    glDeleteRenderbuffers(1, &rbo);
    glDeleteTextures(1, &texture);
    glDeleteTextures(1, &ms_texture);
}

void GL_MultisamplingFramebuffer::setResolution(int width, int height) {
    this->width = width;
    this->width = width;
    this->height = height;

    glBindFramebuffer(GL_FRAMEBUFFER, ms_fbo);

    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, ms_texture);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA8, width, height, GL_TRUE);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, ms_texture, 0);

    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, width, height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        Log::error("Framebuffer is not complete!");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture,
                           0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        Log::error("Framebuffer is not complete!");
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    this->height = height;
}

void GL_MultisamplingFramebuffer::unbindTexture() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}

void GL_MultisamplingFramebuffer::bindTexture() const {
    glBindTexture(GL_TEXTURE_2D, texture);
}

void GL_MultisamplingFramebuffer::bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, ms_fbo);
}

void GL_MultisamplingFramebuffer::unbind() const {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, ms_fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
