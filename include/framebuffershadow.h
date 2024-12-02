#ifndef __FRAMEBUFFERSHADOW_H__
#define __FRAMEBUFFERSHADOW_H__ 1

#include "types.h"
#include <glad/glad.h>
#include "framebuffer.h"


namespace coalengine
{
  class Texture;

  class FrameBufferShadow : public FrameBuffer
  {
  public:
    FrameBufferShadow();
    virtual ~FrameBufferShadow();
    FrameBufferShadow(const FrameBufferShadow& o) = delete;
    FrameBufferShadow(FrameBufferShadow&& o);
    FrameBufferShadow& operator= (const FrameBufferShadow& o) = delete;
    FrameBufferShadow& operator= (FrameBufferShadow&& o);

    void FrameBufferGen() override;
    void DrawFrameBuffer() override;
    uint16 width_shadow_;
    uint16 height_shadow_;
  };

}

#endif
