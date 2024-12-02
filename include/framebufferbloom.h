#ifndef __FRAMEBUFFERBLOOM_H__
#define __FRAMEBUFFERBLOOM_H__ 1

#include "types.h"
#include <glad/glad.h>
#include "framebuffer.h"
#include "geometry.h"


namespace coalengine
{
  class Texture;

  class FrameBufferBloom : public FrameBuffer
  {
  public:
    FrameBufferBloom();
    virtual ~FrameBufferBloom();
    FrameBufferBloom(const FrameBufferBloom& o) = delete;
    FrameBufferBloom(FrameBufferBloom&& o);
    FrameBufferBloom& operator= (const FrameBufferBloom& o) = delete;
    FrameBufferBloom& operator= (FrameBufferBloom&& o);

    void FrameBufferGen() override;
    void DrawFrameBuffer() override;

    // Extract bright color.
    GLuint bloom_buffers_[2];

    // Ping - Pong blur.
    GLuint ping_pong_frame_buffers_[2];
    GLuint ping_pong_textures_[2];

    GLuint ping_pong_frag_id_;
    GLuint ping_pong_vertex_id_;
    GLuint ping_pong_program_;

    std::string vertex_source_ping_pong_;
    std::string fragment_source_ping_pong_;

    // number of iterations on ping-pong buffers.
    uint16 amount_; 
  };

}

#endif
