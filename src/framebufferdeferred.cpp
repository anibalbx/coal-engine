#include "framebufferdeferred.h"

#include "ecs.h"
#include "glm/gtc/type_ptr.hpp"
#include "engineutils.h"
#include "renderlist.h"

coalengine::FrameBufferG::FrameBufferG()
{
  type_ = kDeferred;
  g_position_texture_ = 0;
  g_normal_texture_ = 0;
  g_spec_albedo_texture_ = 0;
  g_ambient_occlusion_texture_ = 0;

  LoadShaders("../../../data/Shaders/PBRDeferred.vert", "../../../data/Shaders/PBRDeferred.frag");
}

coalengine::FrameBufferG::~FrameBufferG()
{
}

coalengine::FrameBufferG::FrameBufferG(FrameBufferG&& o) : FrameBuffer(std::move(o))
{
  g_position_texture_ = o.g_position_texture_;
  g_normal_texture_ = o.g_normal_texture_;
  g_spec_albedo_texture_ = o.g_spec_albedo_texture_;
}

coalengine::FrameBufferG& coalengine::FrameBufferG::operator=(FrameBufferG&& o)
{
  FrameBuffer::operator=(std::move(o));
  g_position_texture_ = o.g_position_texture_;
  g_normal_texture_ = o.g_normal_texture_;
  g_spec_albedo_texture_ = o.g_spec_albedo_texture_;
  return *this;
}

void coalengine::FrameBufferG::FrameBufferGen()
{
  if (!uploaded_)
  {
    glGenFramebuffers(1, &id_);
    glBindFramebuffer(GL_FRAMEBUFFER, id_);

    // Position color buffer
    glGenTextures(1, &g_position_texture_);
    glActiveTexture(GL_TEXTURE0 + g_position_texture_);
    glBindTexture(GL_TEXTURE_2D, g_position_texture_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, EngineUtils::Instance().width,
      EngineUtils::Instance().height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, g_position_texture_, 0);
    

    // Normal color buffer
    glGenTextures(1, &g_normal_texture_);
    glActiveTexture(GL_TEXTURE0 + g_normal_texture_);
    glBindTexture(GL_TEXTURE_2D, g_normal_texture_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, EngineUtils::Instance().width,
      EngineUtils::Instance().height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, g_normal_texture_, 0);


    // Albedo and specular color buffer
    glGenTextures(1, &g_spec_albedo_texture_);
    glActiveTexture(GL_TEXTURE0 + g_spec_albedo_texture_);
    glBindTexture(GL_TEXTURE_2D, g_spec_albedo_texture_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, EngineUtils::Instance().width,
      EngineUtils::Instance().height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, g_spec_albedo_texture_, 0);


    // AO color buffer
    glGenTextures(1, &g_ambient_occlusion_texture_);
    glActiveTexture(GL_TEXTURE0 + g_ambient_occlusion_texture_);
    glBindTexture(GL_TEXTURE_2D, g_ambient_occlusion_texture_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, EngineUtils::Instance().width,
      EngineUtils::Instance().height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, g_ambient_occlusion_texture_, 0);

    // Roughness color buffer
    glGenTextures(1, &g_roughness_texture_);
    glActiveTexture(GL_TEXTURE0 + g_roughness_texture_);
    glBindTexture(GL_TEXTURE_2D, g_roughness_texture_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, EngineUtils::Instance().width,
     EngineUtils::Instance().height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, g_roughness_texture_, 0);


    // Depth buffer
    glGenRenderbuffers(1, &rbo_);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo_);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, EngineUtils::Instance().width, EngineUtils::Instance().height); 
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo_); 

    GLenum attachments[5] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
    glDrawBuffers(5, attachments);

    GLenum error = glGetError();


    if (error != GL_NO_ERROR)
    {
      printf("OPENGL error\n");
    }

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
      printf("Frame buffer error!\n");
    }
    uploaded_ = true;
  }

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glBindFramebuffer(GL_FRAMEBUFFER, id_);

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glEnable(GL_CULL_FACE);
  glFrontFace(GL_CCW);
}

void coalengine::FrameBufferG::DrawFrameBuffer()
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
    //------------------------------------------------------------------------------------------------------------------------------------------------

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
 

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if (EngineUtils::Instance().show_engine_front_)
	glBindFramebuffer(GL_FRAMEBUFFER, id_);

  glUseProgram(program_id_);

  // G-Buffer uniforms
  glUniform1i(glGetUniformLocation(program_id_, "u_gPosition"), g_position_texture_);
  glUniform1i(glGetUniformLocation(program_id_, "u_gNormal"), g_normal_texture_);
  glUniform1i(glGetUniformLocation(program_id_, "u_gAlbedoSpec"), g_spec_albedo_texture_);
  glUniform1i(glGetUniformLocation(program_id_, "u_texture_position"), g_position_texture_);
  glUniform1i(glGetUniformLocation(program_id_, "u_gAmbientOcclusion"), g_ambient_occlusion_texture_);
  glUniform1i(glGetUniformLocation(program_id_, "u_gRoughness"), g_roughness_texture_);

  ECS& ecs = ECS::Instance();

  std::vector<glm::mat4> modelview;

  for(auto it=ecs.light_ent_map_.begin(); it != ecs.light_ent_map_.end(); it++ )
  {
    modelview.push_back(it->second->light_view_ * it->second->model_);
    glUniformMatrix4fv(glGetUniformLocation(program_id_, "u_lightProjection"),1, GL_FALSE, glm::value_ptr(it->second->projection_));
    glUniformMatrix4fv(glGetUniformLocation(program_id_, "u_lightModelView"),modelview.size(), GL_FALSE, glm::value_ptr(modelview[0]));
    
  }
	
  RenderList& RL = RenderList::Instance();

  glUniform3f(glGetUniformLocation(program_id_, "u_camPos"), RL.camera_->position_.x,
    RL.camera_->position_.y,
    RL.camera_->position_.z);

  unsigned int directional_number = RL.lights_.lightdirpos_.size();
  unsigned int point_number = RL.lights_.lightpointpos_.size();
  unsigned int spot_number = RL.lights_.lightspotpos_.size();

  unsigned int directional_number_shadow = RL.lights_.lightdirposshadow_.size();
  unsigned int spot_number_shadow = RL.lights_.lightspotposshadow_.size();

  // ALl light uniforms
  if (!RL.lights_.light_spaces_dir_.empty())
  {
      glUniformMatrix4fv(glGetUniformLocation(program_id_, "u_lightSpaceMatrixDir"), RL.lights_.light_spaces_dir_.size(), GL_FALSE, glm::value_ptr(RL.lights_.light_spaces_dir_[0]));
      glUniform1f(glGetUniformLocation(program_id_, "u_numShadowLightsDir"), RL.lights_.light_spaces_dir_.size());
  }
  if (!RL.lights_.light_spaces_spot_.empty())
  {
      glUniformMatrix4fv(glGetUniformLocation(program_id_, "u_lightSpaceMatrixSpot"), RL.lights_.light_spaces_spot_.size(), GL_FALSE, glm::value_ptr(RL.lights_.light_spaces_spot_[0]));
      glUniform1f(glGetUniformLocation(program_id_, "u_numShadowLightsSpot"), RL.lights_.light_spaces_spot_.size());
  }

  if (directional_number > 0)
  {
    glUniform1f(glGetUniformLocation(program_id_, "u_NumDirectionals"), directional_number);
    glUniform3fv(glGetUniformLocation(program_id_, "u_LightDirPosition"), directional_number, glm::value_ptr(RL.lights_.lightdirpos_[0]));
    glUniform3fv(glGetUniformLocation(program_id_, "u_LightDirDirection"), directional_number, glm::value_ptr(RL.lights_.lightdirdir_[0]));
    glUniform3fv(glGetUniformLocation(program_id_, "u_LightDirColor"), directional_number, glm::value_ptr(RL.lights_.lightdircolor_[0]));
    glUniform1fv(glGetUniformLocation(program_id_, "u_LightDirIntensity"), directional_number, &RL.lights_.lightdirintensity_[0]);
  }else
  {
    glUniform1f(glGetUniformLocation(program_id_, "u_NumDirectionals"), 0);
  }


  if (directional_number_shadow > 0)
  {
      glUniform1f(glGetUniformLocation(program_id_, "u_NumDirectionalsShadow"), directional_number_shadow);
      glUniform3fv(glGetUniformLocation(program_id_, "u_LightDirPositionShadows"), directional_number_shadow, glm::value_ptr(RL.lights_.lightdirposshadow_[0]));
      glUniform3fv(glGetUniformLocation(program_id_, "u_LightDirDirectionShadows"), directional_number_shadow, glm::value_ptr(RL.lights_.lightdirdirshadow_[0]));
      glUniform3fv(glGetUniformLocation(program_id_, "u_LightDirColorShadows"), directional_number_shadow, glm::value_ptr(RL.lights_.lightdircolorshadow_[0]));
      glUniform1iv(glGetUniformLocation(program_id_, "u_shadowMapDir"), RL.lights_.shadow_map_dir_.size(), &RL.lights_.shadow_map_dir_[0]);
      glUniform1fv(glGetUniformLocation(program_id_, "u_LightDirIntensityShadows"), directional_number_shadow, &RL.lights_.lightdirintensityshadow_[0]);
  }else
  {
    glUniform1f(glGetUniformLocation(program_id_, "u_NumDirectionalsShadow"), 0);
  }

  if (point_number > 0)
  {
    glUniform1f(glGetUniformLocation(program_id_, "u_NumPoints"), point_number);
    glUniform3fv(glGetUniformLocation(program_id_, "u_LightPointPosition"), point_number, glm::value_ptr(RL.lights_.lightpointpos_[0]));
    glUniform3fv(glGetUniformLocation(program_id_, "u_LightPointDirection"), point_number, glm::value_ptr(RL.lights_.lightpointdir_[0]));
    glUniform3fv(glGetUniformLocation(program_id_, "u_LightPointColor"), point_number, glm::value_ptr(RL.lights_.lightpointcolor_[0]));
    glUniform1fv(glGetUniformLocation(program_id_, "u_LightPointIntensity"), point_number, &RL.lights_.lightpointintensity_[0]);
  }else
  {
    glUniform1f(glGetUniformLocation(program_id_, "u_NumPoints"), 0);
  }

  if (spot_number > 0)
  {
    glUniform1f(glGetUniformLocation(program_id_, "u_NumSpots"), spot_number);
    glUniform3fv(glGetUniformLocation(program_id_, "u_LightSpotPosition"), spot_number, glm::value_ptr(RL.lights_.lightspotpos_[0]));
    glUniform3fv(glGetUniformLocation(program_id_, "u_LightSpotDirection"), spot_number, glm::value_ptr(RL.lights_.lightspotdir_[0]));
    glUniform3fv(glGetUniformLocation(program_id_, "u_LightSpotColor"), spot_number, glm::value_ptr(RL.lights_.lightspotcolor_[0]));
    glUniform1fv(glGetUniformLocation(program_id_, "u_LightSpotInnerAngle"), spot_number, &RL.lights_.lightspotinnerangle_[0]);
    glUniform1fv(glGetUniformLocation(program_id_, "u_LightSpotOuterAngle"), spot_number, &RL.lights_.lightspotouterangle_[0]);
    glUniform1fv(glGetUniformLocation(program_id_, "u_LightSpotIntensity"), spot_number, &RL.lights_.lightspotintensity_[0]);
  }else
  {
    glUniform1f(glGetUniformLocation(program_id_, "u_NumSpots"), 0);
  }

  if (spot_number_shadow > 0)
  {
      glUniform1f(glGetUniformLocation(program_id_, "u_NumSpotsShadow"), spot_number_shadow);
      glUniform3fv(glGetUniformLocation(program_id_, "u_LightSpotPositionShadows"), spot_number_shadow, glm::value_ptr(RL.lights_.lightspotposshadow_[0]));
      glUniform3fv(glGetUniformLocation(program_id_, "u_LightSpotDirectionShadows"), spot_number_shadow, glm::value_ptr(RL.lights_.lightspotdirshadow_[0]));
      glUniform3fv(glGetUniformLocation(program_id_, "u_LightSpotColorShadows"), spot_number_shadow, glm::value_ptr(RL.lights_.lightspotcolorshadow_[0]));
      glUniform1fv(glGetUniformLocation(program_id_, "u_LightSpotInnerAngleShadows"), spot_number_shadow, &RL.lights_.lightspotinnerangleshadow_[0]);
      glUniform1fv(glGetUniformLocation(program_id_, "u_LightSpotOuterAngleShadows"), spot_number_shadow, &RL.lights_.lightspotouterangleshadow_[0]);
      glUniform1iv(glGetUniformLocation(program_id_, "u_shadowMapSpot"), RL.lights_.shadow_map_spot_.size(), &RL.lights_.shadow_map_spot_[0]);
      glUniform1fv(glGetUniformLocation(program_id_, "u_LightSpotIntensityShadows"), spot_number_shadow, &RL.lights_.lightspotintensityshadow_[0]);
  }else
  {
    glUniform1f(glGetUniformLocation(program_id_, "u_NumSpotsShadow"), 0);
  }

  glBindVertexArray(vao_);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
