#include "framebufferbloom.h"
#include <stdio.h>
#include "engineutils.h"

coalengine::FrameBufferBloom::FrameBufferBloom()
{
  bloom_buffers_[0] = 0;
  bloom_buffers_[1] = 0;
  ping_pong_textures_[0] = 0;
  ping_pong_textures_[1] = 0;
  ping_pong_frag_id_ = 0;
  ping_pong_vertex_id_ = 0;
  ping_pong_frame_buffers_[0] = 0;
  ping_pong_frame_buffers_[1] = 0;
  ping_pong_program_ = 0;

  type_ = kBloom;
  amount_ = 10;
}

coalengine::FrameBufferBloom::~FrameBufferBloom()
{
}

coalengine::FrameBufferBloom::FrameBufferBloom(FrameBufferBloom&& o) : FrameBuffer(std::move(o))
{
  bloom_buffers_[0] = o.bloom_buffers_[0];
  bloom_buffers_[1] = o.bloom_buffers_[1];

}

coalengine::FrameBufferBloom& coalengine::FrameBufferBloom::operator=(FrameBufferBloom&& o)
{
  FrameBuffer::operator=(std::move(o));
  bloom_buffers_[0] = o.bloom_buffers_[0];
  bloom_buffers_[1] = o.bloom_buffers_[1];
  return  *this;
}

void coalengine::FrameBufferBloom::FrameBufferGen()
{
  if (!uploaded_)
  {
    glGenFramebuffers(1, &id_);
    glBindFramebuffer(GL_FRAMEBUFFER, id_);

    glGenTextures(2, bloom_buffers_);


    for (int i = 0; i < 2; ++i)
    {
      glActiveTexture(GL_TEXTURE0 + bloom_buffers_[i]);
      glBindTexture(GL_TEXTURE_2D, bloom_buffers_[i]);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, EngineUtils::Instance().width,
        EngineUtils::Instance().height, 0, GL_RGB, GL_FLOAT, NULL);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, bloom_buffers_[i], 0);
    }

    glGenRenderbuffers(1, &rbo_);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo_);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, EngineUtils::Instance().width, EngineUtils::Instance().height); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo_); // now actually attach it
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now

    GLenum attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);

    GLenum error = glGetError();


    if (error != GL_NO_ERROR)
    {
      printf("OPENGL error\n");
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
      printf("Frame buffer error!\n");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);


    // Ping - Pong buffers
    glGenFramebuffers(2, ping_pong_frame_buffers_);
    glGenTextures(2, ping_pong_textures_);

    for (unsigned int i = 0; i < 2; i++)
    {
      glBindFramebuffer(GL_FRAMEBUFFER, ping_pong_frame_buffers_[i]);
      glActiveTexture(GL_TEXTURE0 + ping_pong_textures_[i]);
      glBindTexture(GL_TEXTURE_2D, ping_pong_textures_[i]);
      glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA16F, EngineUtils::Instance().width, EngineUtils::Instance().height, 0, GL_RGBA, GL_FLOAT, NULL
      );
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ping_pong_textures_[i], 0
      );

      if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      {
        printf("Frame buffer error!\n");
      }
     

    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    uploaded_ = true;
  }




  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0, 0, EngineUtils::Instance().width, EngineUtils::Instance().height);
  glBindFramebuffer(GL_FRAMEBUFFER, id_);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glEnable(GL_CULL_FACE);
  glFrontFace(GL_CCW);
}

void coalengine::FrameBufferBloom::DrawFrameBuffer()
{
  
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, EngineUtils::Instance().width, EngineUtils::Instance().height);
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



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

      #version 330 core
      out vec4 fragColor;

      in vec3 normal;
      in vec2 uv;
      uniform sampler2D my_texture;
      uniform sampler2D my_texture_two;
      uniform bool bloom;
      uniform float exposure;

      void main()
      {
        const float gamma = 2.2;
        vec3 hdrColor = texture(my_texture, uv).rgb;
        vec3 bloomColor = texture(my_texture_two, uv).rgb;
 
        hdrColor += bloomColor;
        //vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
        //result = pow(result, vec3(1.0 / gamma));
        fragColor = vec4(hdrColor, 1.0);
            
      }
     )";

      vertex_source_.append(vertex_source_char);
      fragment_source_.append(fragment_source_char);

    }


    const char vertex_source_char_ping_pong[] = R"(

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

    const char fragment_source_char_ping_pong[] = R"(

      #version 330 core
      //out vec4 fragColor;
  	  layout (location = 0) out vec4 fragColor;

      in vec3 normal;
      in vec2 uv;
      uniform sampler2D my_texture;

      uniform bool horizontal;
      //float weight[5] = float[] (0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
      //float weight[5] = float[] (0.227027, 0.1945946/2, 0.1216216/2, 0.054054/2, 0.016216/2);
      float weight[5] = float[] (0.227027, 0.016216, 0.1216216, 0.054054, 0.016216);

    
 
      void main()
      {
        vec2 tex_offset = 1.0 / textureSize(my_texture, 0); // gets size of single texel
        vec3 result = texture(my_texture, uv).rgb * weight[0]; // current fragment's contribution
        if(horizontal)
        {
          for(int i = 1; i < 5; ++i)
          {
            result += texture(my_texture, uv + vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
            result += texture(my_texture, uv - vec2(tex_offset.x * i, 0.0)).rgb * weight[i];
          }
        }
        else
        {
          for(int i = 1; i < 5; ++i)
          {
            result += texture(my_texture, uv + vec2(0.0, tex_offset.y * i)).rgb * weight[i];
            result += texture(my_texture, uv - vec2(0.0, tex_offset.y * i)).rgb * weight[i];
          }
        }
        fragColor = vec4(result, 1.0);
      }
     )";

    vertex_source_ping_pong_.append(vertex_source_char_ping_pong);
    fragment_source_ping_pong_.append(fragment_source_char_ping_pong);

    // extract bright shaders
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

    // Ping-Pong shaders
    ping_pong_vertex_id_ = glCreateShader(GL_VERTEX_SHADER);

    length = vertex_source_ping_pong_.size();
    c_str = vertex_source_ping_pong_.c_str();
    glShaderSource(ping_pong_vertex_id_, 1, &c_str, &length);
    glCompileShader(ping_pong_vertex_id_);

    ping_pong_frag_id_ = glCreateShader(GL_FRAGMENT_SHADER);

    length = fragment_source_ping_pong_.size();
    c_str = fragment_source_ping_pong_.c_str();
    glShaderSource(ping_pong_frag_id_, 1, &c_str, &length);
    glCompileShader(ping_pong_frag_id_);

    //program
    ping_pong_program_ = glCreateProgram();
    glAttachShader(ping_pong_program_, ping_pong_vertex_id_);
    glAttachShader(ping_pong_program_, ping_pong_frag_id_);
    glLinkProgram(ping_pong_program_);
    glUseProgram(ping_pong_program_);

    glGetShaderiv(ping_pong_vertex_id_, GL_COMPILE_STATUS, &success);
    if (!success)
    {
      printf("compile vertex error\n");
    }

    glGetShaderiv(ping_pong_frag_id_, GL_COMPILE_STATUS, &success);
    if (!success)
    {
      printf("compile fragment error\n");
    }

    glGetProgramiv(ping_pong_program_, GL_LINK_STATUS, &success);
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

  glUseProgram(ping_pong_program_);


  bool first_iteration = true;
  bool horizontal = true;

  for (unsigned int i = 0; i < amount_; i++)
  {
    glBindFramebuffer(GL_FRAMEBUFFER, ping_pong_frame_buffers_[horizontal]);
    glUniform1i(glGetUniformLocation(ping_pong_program_, "horizontal"), horizontal);
    if(first_iteration)
    {
      glUniform1i(glGetUniformLocation(ping_pong_program_, "my_texture"), bloom_buffers_[1]);

    }else
    {
      glUniform1i(glGetUniformLocation(ping_pong_program_, "my_texture"), ping_pong_textures_[!horizontal]);

    }
   

    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


    glBindVertexArray(0);

    horizontal = !horizontal;
    if (first_iteration)
    {
      first_iteration = false;
    }
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

	
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glUseProgram(program_id_);


  if (EngineUtils::Instance().show_engine_front_)
	glBindFramebuffer(GL_FRAMEBUFFER, id_);
	
  GLint text = glGetUniformLocation(program_id_, "my_texture");
  GLint text2 = glGetUniformLocation(program_id_, "my_texture_two");
  glUniform1i(text,bloom_buffers_[0]);
  glUniform1i(text2,ping_pong_textures_[!horizontal]);
  glUniform1i(glGetUniformLocation(program_id_, "bloom"), true);
  glUniform1f(glGetUniformLocation(program_id_, "exposure"), 2.0f);
  glBindVertexArray(vao_);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  GLenum error = glGetError();


  if (error != GL_NO_ERROR)
  {
    printf("OPENGL error");
  }
}
