#include "postprocess.h"


#include "engineutils.h"
#include "renderlist.h"

coalengine::PostProcess::PostProcess()
{
  uploaded_to_gpu_ = false;
  program_id_ = 0;
  vertex_shader_id_ = 0;
  fragment_shader_id_ = 0;
  vao_ = 0;
  vertices_id_ = 0;
  indices_id_ = 0;
  type_ = kNone;
}

coalengine::PostProcess::~PostProcess()
{
  glDeleteProgram(program_id_);
  glDeleteShader(fragment_shader_id_);
  glDeleteShader(vertex_shader_id_);
  glDeleteBuffers(1, &vertices_id_);
  glDeleteBuffers(1, &indices_id_);
}

coalengine::PostProcess::PostProcess(const PostProcess&& o)
{
  uploaded_to_gpu_ = o.uploaded_to_gpu_;
  program_id_ = o.program_id_;
  vertex_shader_id_ = o.vertex_shader_id_;
  fragment_shader_id_ = o.fragment_shader_id_;
  vao_ = o.vao_;
  vertices_id_ = o.vertices_id_;
  indices_id_ = o.indices_id_;
  type_ = o.type_;
}

coalengine::PostProcess& coalengine::PostProcess::operator=(PostProcess&& o)
{
  uploaded_to_gpu_ = o.uploaded_to_gpu_;
  program_id_ = o.program_id_;
  vertex_shader_id_ = o.vertex_shader_id_;
  fragment_shader_id_ = o.fragment_shader_id_;
  vao_ = o.vao_;
  vertices_id_ = o.vertices_id_;
  indices_id_ = o.indices_id_;
  type_ = o.type_;

  return *this;
}

void coalengine::PostProcess::RunPostPro(int iteration)
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  RenderList& RL = RenderList::Instance();

  if (!uploaded_to_gpu_)
  {

    glGenFramebuffers(1, &ping_pong_frame_buffer_);
    glGenTextures(1, &ping_pong_texture_);

    
    glBindFramebuffer(GL_FRAMEBUFFER, ping_pong_frame_buffer_);
    glActiveTexture(GL_TEXTURE0 + ping_pong_texture_);
    glBindTexture(GL_TEXTURE_2D, ping_pong_texture_);
    glTexImage2D(
      GL_TEXTURE_2D, 0, GL_RGBA16F, EngineUtils::Instance().width, EngineUtils::Instance().height, 0, GL_RGBA, GL_FLOAT, NULL
    );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(
      GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ping_pong_texture_, 0
    );

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
      printf("Frame buffer error!\n");
    }
    

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

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
    // Vertex Shader

    if (type_ == kGrayScale)
    {
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
      //out vec4 fragColor;
      in vec3 normal;
      in vec2 uv;
      uniform sampler2D u_texture;

      void main()
      {
        //fragColor = vec4(vec3(1.0 - texture(u_texture, uv)), 1.0);
        
       fragColor = texture(u_texture, uv);
       float average = 0.2126 * fragColor.r + 0.7152 * fragColor.g + 0.0722 * fragColor.b;
       fragColor = vec4(average, average, average, 1.0);
      }
     )";

      vertex_source_.append(vertex_source_char);
      fragment_source_.append(fragment_source_char);
    }
    else if(type_ == kBlur)
    {
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
      //out vec4 fragColor;
      in vec3 normal;
      in vec2 uv;
      uniform sampler2D u_texture;

      const float offset = 1.0 / 300.0; 

      void main()
      {
        vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right    
        );

        float kernel[9] = float[](
        1.0 / 16, 2.0 / 16, 1.0 / 16,
        2.0 / 16, 4.0 / 16, 2.0 / 16,
        1.0 / 16, 2.0 / 16, 1.0 / 16  
        );
    
        vec3 sampleTex[9];
        for(int i = 0; i < 9; i++)
        {
            sampleTex[i] = vec3(texture(u_texture, uv + offsets[i]));
        }
        vec3 col = vec3(0.0);

        for(int i = 0; i < 9; i++)
          col += sampleTex[i] * kernel[i];
    
        fragColor = vec4(col, 1.0);
      }
     )";

      vertex_source_.append(vertex_source_char);
      fragment_source_.append(fragment_source_char);
    }else if(type_ == kKernelEffect)
    {
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
      //out vec4 fragColor;
      in vec3 normal;
      in vec2 uv;
      uniform sampler2D u_texture;

      const float offset = 1.0 / 300.0; 

      void main()
      {
        vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // top-left
        vec2( 0.0f,    offset), // top-center
        vec2( offset,  offset), // top-right
        vec2(-offset,  0.0f),   // center-left
        vec2( 0.0f,    0.0f),   // center-center
        vec2( offset,  0.0f),   // center-right
        vec2(-offset, -offset), // bottom-left
        vec2( 0.0f,   -offset), // bottom-center
        vec2( offset, -offset)  // bottom-right    
        );

        float kernel[9] = float[](
        1, 1, 1,
        1,-8, 1,
        1, 1, 1
    );
    
        vec3 sampleTex[9];
        for(int i = 0; i < 9; i++)
        {
            sampleTex[i] = vec3(texture(u_texture, uv + offsets[i]));
        }
        vec3 col = vec3(0.0);

        for(int i = 0; i < 9; i++)
          col += sampleTex[i] * kernel[i];
    
        fragColor = vec4(col, 1.0);
      }
     )";

      vertex_source_.append(vertex_source_char);
      fragment_source_.append(fragment_source_char);
    }else if(type_ == kInversion)
    {
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
      //out vec4 fragColor;
      in vec3 normal;
      in vec2 uv;
      uniform sampler2D u_texture;

      void main()
      {    
       fragColor = vec4(vec3(1.0 - texture(u_texture, uv)), 1.0);
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

    uploaded_to_gpu_ = true;
  }

  glUseProgram(program_id_);

  if (RL.Instance().post_process_list_.size() == 1)
  {

    if(RenderList::Instance().camera_->bloom_active_)
    {
      glBindFramebuffer(GL_FRAMEBUFFER, RL.bloom_fb_.id_);
      glUniform1i(glGetUniformLocation(program_id_, "u_texture"), RL.bloom_fb_.bloom_buffers_[0]);
    }else
    {
      glBindFramebuffer(GL_FRAMEBUFFER, RL.principal_fb_.id_);
      glUniform1i(glGetUniformLocation(program_id_, "u_texture"), RL.principal_fb_.texture_id_);
    }  
  }else if (iteration == 0)
  {
    if (RenderList::Instance().camera_->bloom_active_)
    {
      glBindFramebuffer(GL_FRAMEBUFFER, ping_pong_frame_buffer_);
      glUniform1i(glGetUniformLocation(program_id_, "u_texture"), RL.bloom_fb_.bloom_buffers_[0]);
    }else
    {
      glBindFramebuffer(GL_FRAMEBUFFER, ping_pong_frame_buffer_);
      glUniform1i(glGetUniformLocation(program_id_, "u_texture"), RL.principal_fb_.texture_id_);
    }
   
  }else if(iteration == RL.post_process_list_.size() - 1)
  {
    if (RenderList::Instance().camera_->bloom_active_)
    {
      glBindFramebuffer(GL_FRAMEBUFFER, RL.bloom_fb_.id_);
      glUniform1i(glGetUniformLocation(program_id_, "u_texture"), RL.post_process_list_.at(iteration - 1)->ping_pong_texture_);
    }
    else
    {
      glBindFramebuffer(GL_FRAMEBUFFER, RL.principal_fb_.id_);
      glUniform1i(glGetUniformLocation(program_id_, "u_texture"), RL.post_process_list_.at(iteration - 1)->ping_pong_texture_);
    }  
  }else
  {
    glBindFramebuffer(GL_FRAMEBUFFER, ping_pong_frame_buffer_);
    glUniform1i(glGetUniformLocation(program_id_, "u_texture"), RL.post_process_list_.at(iteration - 1)->ping_pong_texture_);
  }

  glBindVertexArray(vao_);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindVertexArray(0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    printf("FRAMEBUFFER ERROR");
}
