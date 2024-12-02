#include "framebuffer.h"
#include "engineutils.h"
#include "RenderList.h"
#include "texturebuilder.h"


coalengine::FrameBuffer::FrameBuffer()
{
  id_ = 0;
  texture_id_ = 0;
  uploaded_ = false;
  geo_uploaded_ = false;
  vao_ = 0;
  vertices_id_= 0;
  indices_id_= 0;
  vertex_shader_id_= 0;
  fragment_shader_id_= 0;
  program_id_= 0;
  type_ = kNormalFB;
}

coalengine::FrameBuffer::~FrameBuffer()
{
  glDeleteFramebuffers(1, &id_);
  glDeleteShader(vertex_shader_id_);
  glDeleteShader(fragment_shader_id_);
  glDeleteProgram(program_id_);
  glDeleteBuffers(1, &vertices_id_);
  glDeleteBuffers(1, &indices_id_);
  glDeleteTextures(1, &texture_id_);
}

coalengine::FrameBuffer::FrameBuffer(FrameBuffer&& o)
{
  id_ = o.id_;
  texture_id_ = o.texture_id_;
  uploaded_ = o.uploaded_;
  geo_uploaded_ = o.geo_uploaded_;
  vao_ = o.vao_;
  vertices_id_ = o.vertices_id_;
  indices_id_ = o.indices_id_;
  vertex_shader_id_ = o.vertex_shader_id_;
  fragment_shader_id_ = o.fragment_shader_id_;
  program_id_ = o.program_id_;
  type_ = o.type_;

}

coalengine::FrameBuffer& coalengine::FrameBuffer::operator=(FrameBuffer&& o)
{
  id_ = o.id_;
  texture_id_ = o.texture_id_;
  uploaded_ = o.uploaded_;
  geo_uploaded_ = o.geo_uploaded_;
  vao_ = o.vao_;
  vertices_id_ = o.vertices_id_;
  indices_id_ = o.indices_id_;
  vertex_shader_id_ = o.vertex_shader_id_;
  fragment_shader_id_ = o.fragment_shader_id_;
  program_id_ = o.program_id_;
  type_ = o.type_;
  return  *this;
}

void coalengine::FrameBuffer::FrameBufferGen()
{

    if (!uploaded_)
    {
      glGenFramebuffers(1, &id_);
      glBindFramebuffer(GL_FRAMEBUFFER, id_);


      glGenTextures(1, &texture_id_);
      glActiveTexture(GL_TEXTURE0 + texture_id_);
      glBindTexture(GL_TEXTURE_2D, texture_id_);

      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, EngineUtils::Instance().width,
        EngineUtils::Instance().height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture_id_, 0);

      glGenRenderbuffers(1, &rbo_);
      glBindRenderbuffer(GL_RENDERBUFFER, rbo_);
      glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, EngineUtils::Instance().width, EngineUtils::Instance().height); // use a single renderbuffer object for both a depth AND stencil buffer.
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo_);

      GLenum attachment[1] = {GL_COLOR_ATTACHMENT0};
      glDrawBuffers(1,attachment);

      if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      {
        printf("Frame buffer error!\n");
      }
      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      uploaded_ = true;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, id_);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
}

void coalengine::FrameBuffer::DrawFrameBuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Plane geometry
    if (!geo_uploaded_)
    {
      float vertex[] =
      {
          -1.0f,-1.0f,0.0f,
          0.0f, 1.0f, 0.0f,
          0.0f,0.0f,

          1.0f,-1.0f, 0.0f,
          1.0f, 0.0f, 0.0f,
          1.0f,0.0f,

          -1.0f, 1.0f, 0.0f,
          0.0f, 0.0f, 1.0f,
          0.0f,1.0f,

          1.0f, 1.0f, 0.0f,
          0.0f, 0.0f, 1.0f,
          1.0f,1.0f,
      };

      unsigned int index[] =
      {
          0, 1, 2,
          1, 3, 2,
      };
      glDisable(GL_DEPTH_TEST);
      glGenVertexArrays(1, &vao_);
      glBindVertexArray(vao_);


      glGenBuffers(1, &vertices_id_);
      glBindBuffer(GL_ARRAY_BUFFER, vertices_id_);
      glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_DYNAMIC_DRAW);

      glGenBuffers(1, &indices_id_);
      glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_id_);
      glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_DYNAMIC_DRAW);

      if (vertex_source_.empty() || fragment_source_.empty())
      {
        // Vertex Shader
        const char vertex_source_char[] = R"(

       #version 330
       layout (location = 0) in vec3 a_position;
       layout (location = 1) in vec3 a_normal;
       layout (location = 2) in vec2 a_uv;
       out vec3 normal;
       out vec2 uv;
       void main()
       {
       gl_Position =  vec4(a_position,1.0);
       normal = a_normal;
       uv = a_uv;
       }

     )";

        const char fragment_source_char[] = R"(

      #version 330
      layout (location = 0) out vec4 fragColor;
      in vec3 normal;
      in vec2 uv;
      uniform sampler2D my_texture;
      uniform bool u_activebloom;

      void main()
      {
        vec3 result = texture(my_texture, uv).rgb; 

        if(u_activebloom)
        {
          vec3 hdrColor = texture(my_texture, uv).rgb;
          vec3 tmp = vec3(1.0) - exp(-hdrColor * 2.0);
          result = pow(tmp, vec3(1.0 / 2.2));
        }

        fragColor = vec4(result, 1.0);
      }
     )";

        vertex_source_.append(vertex_source_char);
        fragment_source_.append(fragment_source_char);
      }

      vertex_shader_id_ = glCreateShader(GL_VERTEX_SHADER);

      int length = vertex_source_.size();
      const char* c_str = vertex_source_.c_str();
      glShaderSource(vertex_shader_id_, 1, &c_str, &length);
      glCompileShader(vertex_shader_id_);

      fragment_shader_id_ = glCreateShader(GL_FRAGMENT_SHADER);

      length = fragment_source_.size();
      c_str = fragment_source_.c_str();
      glShaderSource(fragment_shader_id_, 1, &c_str, &length);
      glCompileShader(fragment_shader_id_);

      //program
      program_id_ = glCreateProgram();
      glAttachShader(program_id_, vertex_shader_id_);
      glAttachShader(program_id_, fragment_shader_id_);
      glLinkProgram(program_id_);
      glUseProgram(program_id_);

      // Compile and link check
      int success;

      glGetShaderiv(vertex_shader_id_, GL_COMPILE_STATUS, &success);
      if (!success)
      {
        printf("compile vertex error\n");
      }

      glGetShaderiv(fragment_shader_id_, GL_COMPILE_STATUS, &success);
      if (!success)
      {
        printf("compile fragment error\n");
      }

      glGetProgramiv(program_id_, GL_LINK_STATUS, &success);
      if (!success)
      {

        printf("Program link Error\n");
      }

      //Attrib location
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 3));
      glEnableVertexAttribArray(1);
      glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(sizeof(float) * 6));
      glEnableVertexAttribArray(2);
      glBindVertexArray(0);

      geo_uploaded_ = true;
    }

    glUseProgram(program_id_);
    GLint text = glGetUniformLocation(program_id_, "my_texture");
    glUniform1i(glGetUniformLocation(program_id_, "u_activebloom"),
     RenderList::Instance().camera_->bloom_active_);
    glUniform1i(text, texture_id_);
    glBindVertexArray(vao_);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

  GLenum error = glGetError();

  if(error != GL_NO_ERROR)
  {
    printf("OPENGL error");
  }
}

void coalengine::FrameBuffer::LoadShaders(const char* vertex, const char* fragment)
{
  EngineUtils::Instance().LoadStringFromFile(vertex, vertex_source_);
  EngineUtils::Instance().LoadStringFromFile(fragment, fragment_source_);
}
