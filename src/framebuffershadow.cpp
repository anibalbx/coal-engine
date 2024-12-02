#include "framebuffershadow.h"
#include <stdio.h>
#include "engineutils.h"

coalengine::FrameBufferShadow::FrameBufferShadow()
{
  type_ = kShadowFB;
  width_shadow_ = 4096;
  height_shadow_ = 4096;
}

coalengine::FrameBufferShadow::~FrameBufferShadow()
{
}

coalengine::FrameBufferShadow::FrameBufferShadow(FrameBufferShadow&& o) : FrameBuffer(std::move(o))
{
}

coalengine::FrameBufferShadow& coalengine::FrameBufferShadow::operator=(FrameBufferShadow&& o)
{
  FrameBuffer::operator=(std::move(o));
  return *this;
}

void coalengine::FrameBufferShadow::FrameBufferGen()
{


  if (!uploaded_)
  {
    // configure depth map FBO

    glGenFramebuffers(1, &id_);
    // create depth texture
    glGenTextures(1, &texture_id_);
    glActiveTexture(GL_TEXTURE0 + texture_id_);
    glBindTexture(GL_TEXTURE_2D, texture_id_);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width_shadow_, height_shadow_, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // attach depth texture to FBO
    glBindFramebuffer(GL_FRAMEBUFFER, id_);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture_id_, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    uploaded_ = true;
  }


  glViewport(0, 0, width_shadow_, height_shadow_);
  glBindFramebuffer(GL_FRAMEBUFFER, id_);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glFrontFace(GL_CW);
  glClear(GL_DEPTH_BUFFER_BIT);
}

void coalengine::FrameBufferShadow::DrawFrameBuffer()
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, EngineUtils::Instance().width, EngineUtils::Instance().height);
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  GLenum error = glGetError();

  if (error != GL_NO_ERROR)
  {
    printf("OPENGL error");
  }

}
