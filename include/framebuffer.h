#ifndef __FRAMEBUFFER_H__
#define __FRAMEBUFFER_H__ 1

#include "types.h"
#include <glad/glad.h>
#include "geometry.h"


namespace coalengine
{
  class Texture;

  class FrameBuffer
  {
  public:
    FrameBuffer();
   virtual ~FrameBuffer();
    FrameBuffer(const FrameBuffer& o) = delete;
    FrameBuffer(FrameBuffer&& o);
    FrameBuffer& operator= (const FrameBuffer& o) = delete;
    FrameBuffer& operator= (FrameBuffer&& o);


    virtual void FrameBufferGen();
    virtual void DrawFrameBuffer();
    void LoadShaders(const char* vertex, const char* fragment);

    // Different types of framebuffers depending on the current state of the render.
    enum FbType
    {
      kNone,
      kNormalFB,
      kShadowFB,
      kHDR,
      kBloom,
      kDeferred,
    };

    FbType type_;
    GLuint id_;
    GLuint rbo_;
    GLuint texture_id_;
    bool uploaded_;

    std::string vertex_source_; /**< vertex shader source */
    std::string fragment_source_; /**< fragment shader source */

    bool geo_uploaded_;
    GLuint vao_;
    GLuint vertices_id_;
    GLuint indices_id_;
    GLuint vertex_shader_id_;
    GLuint fragment_shader_id_;
    GLuint program_id_;
  };
}

#endif
