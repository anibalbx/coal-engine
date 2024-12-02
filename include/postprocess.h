#ifndef __POST_PROCESS_H__
#define  __POST_PROCESS_H__ 1
#include <string>
#include <glad/glad.h>

namespace coalengine
{


  class PostProcess
  {
  public:

    enum PostProcessType
    {
      kGrayScale,
      kInversion,
      kBlur,
      kKernelEffect,
      kNone,
    };

    PostProcess();
    ~PostProcess();

    PostProcess(const PostProcess& o) = delete;
    PostProcess& operator=(PostProcess& o) = delete;

    PostProcess(const PostProcess&& o);
    PostProcess& operator=(PostProcess&& o);

    void RunPostPro(int iteration);

    bool uploaded_to_gpu_;

    GLuint program_id_;
    GLuint vertex_shader_id_;
    GLuint fragment_shader_id_;
    std::string vertex_source_;
    std::string fragment_source_;

    GLuint vao_;
    GLuint vertices_id_;
    GLuint indices_id_;
    PostProcessType type_;

    GLuint ping_pong_frame_buffer_;
    GLuint ping_pong_texture_;
  };


}

#endif

