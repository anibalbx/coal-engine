#include "drawcubemap.h"

#include <stb_image.h>
#include "engineutils.h"
#include "renderlist.h"
#include "glm/gtc/type_ptr.hpp"

coalengine::DrawCubeMap::DrawCubeMap()
{

}

coalengine::DrawCubeMap::~DrawCubeMap()
{

}

void coalengine::DrawCubeMap::run()
{
  RenderList& RL = RenderList::Instance();


  if (!RL.sky_box_.uploaded_to_gpu_)
  {
    float skyboxVertices[] = {
      // positions          
      -1.0f,  1.0f, -1.0f,
      -1.0f, -1.0f, -1.0f,
       1.0f, -1.0f, -1.0f,
       1.0f, -1.0f, -1.0f,
       1.0f,  1.0f, -1.0f,
      -1.0f,  1.0f, -1.0f,
        
      -1.0f, -1.0f,  1.0f,
      -1.0f, -1.0f, -1.0f,
      -1.0f,  1.0f, -1.0f,
      -1.0f,  1.0f, -1.0f,
      -1.0f,  1.0f,  1.0f,
      -1.0f, -1.0f,  1.0f,
        
       1.0f, -1.0f, -1.0f,
       1.0f, -1.0f,  1.0f,
       1.0f,  1.0f,  1.0f,
       1.0f,  1.0f,  1.0f,
       1.0f,  1.0f, -1.0f,
       1.0f, -1.0f, -1.0f,
        
      -1.0f, -1.0f,  1.0f,
      -1.0f,  1.0f,  1.0f,
       1.0f,  1.0f,  1.0f,
       1.0f,  1.0f,  1.0f,
       1.0f, -1.0f,  1.0f,
      -1.0f, -1.0f,  1.0f,
        
      -1.0f,  1.0f, -1.0f,
       1.0f,  1.0f, -1.0f,
       1.0f,  1.0f,  1.0f,
       1.0f,  1.0f,  1.0f,
      -1.0f,  1.0f,  1.0f,
      -1.0f,  1.0f, -1.0f,
        
      -1.0f, -1.0f, -1.0f,
      -1.0f, -1.0f,  1.0f,
       1.0f, -1.0f, -1.0f,
       1.0f, -1.0f, -1.0f,
      -1.0f, -1.0f,  1.0f,
       1.0f, -1.0f,  1.0f
    };

    EngineUtils::Instance().GenerateCube(RL.sky_box_.vertices_, RL.sky_box_.normals_, RL.sky_box_.uv_, RL.sky_box_.index_);

    glGenVertexArrays(1, &RL.sky_box_.vao_);
    glBindVertexArray(RL.sky_box_.vao_);

    uint32_t size = RL.sky_box_.vertices_.size() * sizeof(float) + RL.sky_box_.normals_.size() * sizeof(float) + RL.sky_box_.uv_.size() * sizeof(float);

    glGenBuffers(1, &RL.sky_box_.triangle_vertices_ID);

    glBindBuffer(GL_ARRAY_BUFFER, RL.sky_box_.triangle_vertices_ID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);




    //Attrib location
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(RL.sky_box_.vertices_.size() * sizeof(float)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)((RL.sky_box_.vertices_.size() * sizeof(float) + RL.sky_box_.normals_.size() * sizeof(float))));

    //Attrib location
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);


    glGenTextures(1, &RL.sky_box_.texture_id_);
    glActiveTexture(GL_TEXTURE0 + RL.sky_box_.texture_id_);
    glBindTexture(GL_TEXTURE_CUBE_MAP, RL.sky_box_.texture_id_);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < RL.sky_box_.textures.size(); i++)
    {
      unsigned char* data = stbi_load(RL.sky_box_.textures[i].c_str(), &width, &height, &nrChannels, 0);
      if (data)
      {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
          0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
        );
        stbi_image_free(data);
      }
      else
      {
        printf("Cubemap tex failed to load at path: %s", RL.sky_box_.textures[i]);
        stbi_image_free(data);
      }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


    // Vertex Shader
    const char vertex_source_char[] = R"(

       #version 330
       layout (location = 0) in vec3 a_position;
       layout (location = 1) in vec3 a_normal;
       layout (location = 2) in vec2 a_uv;

       out vec3 TexCoords;

       uniform mat4 projection;
       uniform mat4 view;
        
       void main()
       {
          TexCoords = a_position;
          vec4 pos = projection * view * vec4(a_position,1.0);
          gl_Position = pos.xyww;
       }

     )";

    const char fragment_source_char[] = R"(

      #version 330

      out vec4 fragColor;

      in vec3 TexCoords;

      uniform samplerCube skyBox;

      uniform sampler2D my_texture;

      void main()
      {
        fragColor = texture(skyBox, TexCoords); 
      }
     )";

    RL.sky_box_.vertex_source_.append(vertex_source_char);
    RL.sky_box_.fragment_source_.append(fragment_source_char);


    RL.sky_box_.vertex_shader_id_ = glCreateShader(GL_VERTEX_SHADER);

    int length = RL.sky_box_.vertex_source_.size();
    const char* c_str = RL.sky_box_.vertex_source_.c_str();
    glShaderSource(RL.sky_box_.vertex_shader_id_, 1, &c_str, &length);
    glCompileShader(RL.sky_box_.vertex_shader_id_);

    RL.sky_box_.fragment_shader_id_ = glCreateShader(GL_FRAGMENT_SHADER);

    length = RL.sky_box_.fragment_source_.size();
    c_str = RL.sky_box_.fragment_source_.c_str();
    glShaderSource(RL.sky_box_.fragment_shader_id_, 1, &c_str, &length);
    glCompileShader(RL.sky_box_.fragment_shader_id_);

    //program
    RL.sky_box_.program_id_ = glCreateProgram();
    glAttachShader(RL.sky_box_.program_id_, RL.sky_box_.vertex_shader_id_);
    glAttachShader(RL.sky_box_.program_id_, RL.sky_box_.fragment_shader_id_);
    glLinkProgram(RL.sky_box_.program_id_);
    glUseProgram(RL.sky_box_.program_id_);

    // Compile and link check
    int success;

    glGetShaderiv(RL.sky_box_.vertex_shader_id_, GL_COMPILE_STATUS, &success);
    if (!success)
    {
      printf("compile vertex error\n");
    }

    glGetShaderiv(RL.sky_box_.fragment_shader_id_, GL_COMPILE_STATUS, &success);
    if (!success)
    {
      printf("compile fragment error\n");
    }

    glGetProgramiv(RL.sky_box_.program_id_, GL_LINK_STATUS, &success);
    if (!success)
    {

      printf("Program link Error\n");
    }

    RL.sky_box_.uploaded_to_gpu_ = true;

  }

  glDepthFunc(GL_LEQUAL);
  glUseProgram(RL.sky_box_.program_id_);

  glm::mat4 view = glm::mat4(glm::mat3(RL.camera_->view_));
  glUniformMatrix4fv(glGetUniformLocation(RL.sky_box_.program_id_, "view"), 1, GL_FALSE, glm::value_ptr(view));
  glUniformMatrix4fv(glGetUniformLocation(RL.sky_box_.program_id_, "projection"), 1, GL_FALSE, glm::value_ptr(RL.camera_->projection_));
  glUniform1i(glGetUniformLocation(RL.sky_box_.program_id_, "skyBox"), RL.sky_box_.texture_id_);

  glBindVertexArray(RL.sky_box_.vao_);

  glDrawArrays(GL_TRIANGLES, 0, 36);

  glBindVertexArray(0);

  glDepthFunc(GL_LESS);

}
