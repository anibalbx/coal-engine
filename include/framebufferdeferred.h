#ifndef __FRAME_BUFFER_DEFERRED_H__
#define __FRAME_BUFFER_DEFERRED_H__ 1

#include "framebuffer.h"

namespace coalengine
{
  class Texture;

  class FrameBufferG : public FrameBuffer
  {
  public:
    FrameBufferG();
    virtual ~FrameBufferG();
    FrameBufferG(const FrameBufferG& o) = delete;
    FrameBufferG(FrameBufferG&& o);
    FrameBufferG& operator= (const FrameBufferG& o) = delete;
    FrameBufferG& operator= (FrameBufferG&& o);


    void FrameBufferGen() override;
    void DrawFrameBuffer() override;

    // G-Buffer.
    GLuint g_position_texture_;
    GLuint g_normal_texture_;
    GLuint g_spec_albedo_texture_;
    GLuint g_ambient_occlusion_texture_;
    GLuint g_roughness_texture_;
  };
}


#endif

