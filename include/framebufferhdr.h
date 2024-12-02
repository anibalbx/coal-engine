#ifndef __FRAMEBUFFERHDR_H__
#define __FRAMEBUFFERHDR_H__ 1

#include "types.h"
#include <glad/glad.h>
#include "framebuffer.h"
#include "geometry.h"


namespace coalengine
{
  class Texture;

  class FrameBufferHDR : public FrameBuffer
  {
  public:
    FrameBufferHDR();
    virtual ~FrameBufferHDR();
    FrameBufferHDR(const FrameBufferHDR& o) = delete;
    FrameBufferHDR(FrameBufferHDR&& o);
    FrameBufferHDR& operator= (const FrameBufferHDR& o) = delete;
    FrameBufferHDR& operator= (FrameBufferHDR&& o);

    void FrameBufferGen() override;
    void DrawFrameBuffer() override;
  };

}

#endif
