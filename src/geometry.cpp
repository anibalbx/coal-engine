#include "geometry.h"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include "deferredmaterial.h"
#include "lightcomponent.h"
#include "texture.h"
#include "glm/gtc/type_ptr.hpp"
#include "materialbuilder.h"
#include "materialdiffuse.h"
#include "materialpbr.h"
#include "materialphong.h"
#include "renderlist.h"

coalengine::Geometry::Geometry()
{
  uploaded_to_gpu_ = false;
  id_ = 0;
  mat_ = nullptr;
  shape_ = Shape::kOther;
}

coalengine::Geometry::Geometry(Geometry&& other)
{
  vertices_ = std::move(other.vertices_);
  uv_ = std::move(other.uv_);
  index_ = std::move(other.index_);
  normals_ = std::move(other.normals_);
  id_ = other.id_;
  other.id_ = 0;
  uploaded_to_gpu_ = other.uploaded_to_gpu_;
  mat_ = other.mat_;
  other.uploaded_to_gpu_ = false;
  shape_ = other.shape_;
  other.shape_ = Shape::kOther;

}

coalengine::Geometry::~Geometry()
{
  glDeleteBuffers(1, &triangle_indices_ID);
  glDeleteBuffers(1, &triangle_vertices_ID);
}

coalengine::Geometry& coalengine::Geometry::operator=(Geometry&& other)
{
  vertices_.clear();
  normals_.clear();
  uv_.clear();
  index_.clear();
  vertices_ = std::move(other.vertices_);
  normals_ = std::move(other.normals_);
  uv_ = std::move(other.uv_);
  index_ = std::move(other.index_);
  id_ = other.id_;
  other.id_ = 0;
  uploaded_to_gpu_ = other.uploaded_to_gpu_;
  mat_ = other.mat_;
  other.uploaded_to_gpu_ = false;
  shape_ = other.shape_;
  other.shape_ = Shape::kOther;

  return *this;
}

void coalengine::Geometry::RenderGeometry()
{
  RenderList& RL = RenderList::Instance();

  if (!uploaded_to_gpu_)
  {

    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);

    uint32 size = vertices_.size() * sizeof(float) + normals_.size() * sizeof(float) + uv_.size() * sizeof(float);

    glGenBuffers(1, &triangle_vertices_ID);
    glBindBuffer(GL_ARRAY_BUFFER, triangle_vertices_ID);
    glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices_.size() * sizeof(float), vertices_.data());
    glBufferSubData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(float), normals_.size() * sizeof(float), normals_.data());
    glBufferSubData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(float) + normals_.size() * sizeof(float), uv_.size() * sizeof(float), uv_.data());

    glGenBuffers(1, &triangle_indices_ID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangle_indices_ID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_.size() * sizeof(unsigned int), &index_[0], GL_STATIC_DRAW);

    //Attrib location
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(vertices_.size() * sizeof(float)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)((vertices_.size() * sizeof(float) + normals_.size() * sizeof(float))));

    glBindVertexArray(0);

    uploaded_to_gpu_ = true;
  }

  // Apply the shadow shader and generate all shadow maps
  if (RL.state_ == RenderList::kShadowRenderST)
  {
    if (RL.material_shadow_ == 0)
    {
      px::Mem<MaterialBuilder> MB;
      MB.alloc();
      MB->CreateMaterial(Material::kMaterial_Shadow);
      RL.material_shadow_ = MB->Finish();
    }
    Material* shadow_mat = RL.material_map_[RL.material_shadow_].get();
    shadow_mat->UploadMaterial();

    GLint lsmat = glGetUniformLocation(shadow_mat->program_id_, "lightSpaceMatrix");
    GLint model_f = glGetUniformLocation(shadow_mat->program_id_, "model");

    glUseProgram(shadow_mat->program_id_);

    //TODO change when more shadows
    if (RL.lights_.light_spaces_.size() > 0)
    {
      uint8_t shadow_to_render = RenderList::Instance().actual_s_light_ - 1;
      glUniformMatrix4fv(lsmat, 1, GL_FALSE, glm::value_ptr(RL.lights_.light_spaces_[shadow_to_render % RL.lights_.light_spaces_.size()]));
    }

    glUniformMatrix4fv(model_f, 1, GL_FALSE, glm::value_ptr(transform_));
  }
  else if (RL.state_ == RenderList::kDeferredRenderST) // Apply the deferred shader to store the information on the G-Buffer
  {

    GLenum error = glGetError();

    if (error != GL_NO_ERROR)
    {
      printf("OPENGL error");
    }

    if (RL.material_deferred_ == 0)
    {
      px::Mem<MaterialBuilder> MB;
      MB.alloc();
      MB->CreateMaterial(Material::kMaterial_Deferred);
      RL.material_deferred_ = MB->Finish();
    }

    Material* deferred_mat = RL.material_map_[RL.material_deferred_].get();
    deferred_mat->UploadMaterial();

    glBindFramebuffer(GL_FRAMEBUFFER, RenderList::Instance().deferred_fb_.id_);
    glUseProgram(deferred_mat->program_id_);

    RL.camera_->view_ = glm::lookAt(RL.camera_->position_,
      RL.camera_->position_ + RL.camera_->front_,
      RL.camera_->up_);

    glm::mat4 mvp_matrix = RL.camera_->projection_ * RL.camera_->view_ * transform_;

    glUniformMatrix4fv(glGetUniformLocation(deferred_mat->program_id_, "u_mvp"), 1, GL_FALSE, glm::value_ptr(mvp_matrix));
    glUniformMatrix4fv(glGetUniformLocation(deferred_mat->program_id_, "u_model"), 1, GL_FALSE, glm::value_ptr(transform_));
    glUniformMatrix4fv(glGetUniformLocation(deferred_mat->program_id_, "u_view"), 1, GL_FALSE, glm::value_ptr(RL.camera_->view_));

    Texture* aux_text = nullptr;

    if (mat_->GetTexture() != nullptr)
    {
      aux_text = mat_->GetTexture();

      if (aux_text != nullptr)
      {
        aux_text->UploadTexture();
        glUniform1i(glGetUniformLocation(deferred_mat->program_id_, "texture_diffuse1"), mat_->GetTexture()->texture_id_);
      }
    }
    else
    {
      glUniform1i(glGetUniformLocation(deferred_mat->program_id_, "texture_diffuse1"), 0);
    }

    if (mat_->GetMetallicMap() != nullptr)
    {
      aux_text = mat_->GetMetallicMap();

      if (aux_text != nullptr)
      {
        aux_text->UploadTexture();
        glUniform1i(glGetUniformLocation(deferred_mat->program_id_, "texture_specular1"), mat_->GetMetallicMap()->texture_id_);
      }
    }
    else
    {
      glUniform1i(glGetUniformLocation(deferred_mat->program_id_, "texture_specular1"), 0);
    }

    if (mat_->GetRoughnessMap() != nullptr)
    {
      aux_text = mat_->GetRoughnessMap();

      if (aux_text != nullptr)
      {
        aux_text->UploadTexture();
        glUniform1i(glGetUniformLocation(deferred_mat->program_id_, "texture_roughness"), mat_->GetRoughnessMap()->texture_id_);
      }
    }
    else
    {
      glUniform1i(glGetUniformLocation(deferred_mat->program_id_, "texture_roughness"), 0);
    }


    if (mat_->GetAOMap() != nullptr)
    {
      aux_text = mat_->GetAOMap();

      if (aux_text != nullptr)
      {
        aux_text->UploadTexture();
        glUniform1i(glGetUniformLocation(deferred_mat->program_id_, "texture_ao"), mat_->GetAOMap()->texture_id_);
      }
    }
    else
    {
      glUniform1i(glGetUniformLocation(deferred_mat->program_id_, "texture_ao"), 0);
    }

    if (mat_->GetNormalMap() != nullptr)
    {
      aux_text = mat_->GetNormalMap();

      if (aux_text != nullptr)
      {
        aux_text->UploadTexture();
        glUniform1i(glGetUniformLocation(deferred_mat->program_id_, "u_normalMap"), mat_->GetNormalMap()->texture_id_);
        glUniform1i(glGetUniformLocation(deferred_mat->program_id_, "u_HasNormalMap"), true);

      }
    }
    else
    {
      glUniform1i(glGetUniformLocation(deferred_mat->program_id_, "u_normalMap"), 0);
      glUniform1i(glGetUniformLocation(deferred_mat->program_id_, "u_HasNormalMap"), false);
    }

    glUniform3f(glGetUniformLocation(deferred_mat->program_id_, "u_color"), mat_->GetColor().x, mat_->GetColor().y, mat_->GetColor().z);

    error = glGetError();
    if (error != GL_NO_ERROR)
    {
      printf("OPENGL error");
    }
  }
  else if (RL.state_ == RenderList::kNormalRenderST || RL.state_ == RenderList::kHDRRenderST
    || RL.state_ == RenderList::kBloomRenderST) // Regular forward render of all geometries.
  {


    RL.camera_->view_ = glm::lookAt(RL.camera_->position_,
      RL.camera_->position_ + RL.camera_->front_,
      RL.camera_->up_);

    glm::mat4 mvp_matrix = RL.camera_->projection_ * RL.camera_->view_ * transform_;

    float color_float[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

    if (mat_ == nullptr)
    {
      if (RL.material_default_ == 0)
      {
        px::Mem<MaterialBuilder> MB;
        MB.alloc();
        MB->CreateMaterial(Material::kMaterial_Diffuse);
        MB->SetColor(100, 100, 100, 255);
        RL.material_default_ = MB->Finish();
      }
      mat_ = RL.material_map_[RL.material_default_].get();
    }

    mat_->UploadMaterial();

    if (mat_->GetTexture() != nullptr)
    {
      mat_->GetTexture()->UploadTexture();
    }

    if (mat_->GetNormalMap() != nullptr)
    {
      mat_->GetNormalMap()->UploadTexture();
    }

    if (mat_->GetMetallicMap() != nullptr)
    {
      mat_->GetMetallicMap()->UploadTexture();

    }

    if (mat_->GetRoughnessMap() != nullptr)
    {
      mat_->GetRoughnessMap()->UploadTexture();

    }


    if (mat_->GetAOMap() != nullptr)
    {
      mat_->GetAOMap()->UploadTexture();

    }

    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
    {
      printf("OPENGL error");
    }

    GLint loc = glGetUniformLocation(mat_->program_id_, "position");
    GLint color = glGetUniformLocation(mat_->program_id_, "u_color");
    GLint mvp = glGetUniformLocation(mat_->program_id_, "u_mvp");
    GLint model = glGetUniformLocation(mat_->program_id_, "u_model");
    GLint text = glGetUniformLocation(mat_->program_id_, "my_texture");
    GLint metallic = glGetUniformLocation(mat_->program_id_, "u_metallic");
    GLint roughness = glGetUniformLocation(mat_->program_id_, "u_roughness");
    GLint ambient_occlusion = glGetUniformLocation(mat_->program_id_, "u_ambient_occlusion");

    glUseProgram(mat_->program_id_);

    switch (mat_->mat_type_)
    {
    case Material::kNone:

      break;
    case Material::kMaterial_Diffuse:

      MaterialDiffuse* matd;
      matd = dynamic_cast<MaterialDiffuse*>(mat_);

      color_float[0] = (float)matd->GetColor()[0] * 0.003921f;
      color_float[1] = (float)matd->GetColor()[1] * 0.003921f;
      color_float[2] = (float)matd->GetColor()[2] * 0.003921f;
      color_float[3] = (float)matd->GetColor()[3] * 0.003921f;

      glUniform1i(glGetUniformLocation(mat_->program_id_, "u_emissive"), matd->GetIsEmissive());
      glUniform1i(glGetUniformLocation(mat_->program_id_, "u_activebloom"),
        RL.camera_->bloom_active_);

      break;
    case Material::kMaterial_PBR:
      MaterialPBR* matpbr;
      matpbr = dynamic_cast<MaterialPBR*>(mat_);

      color_float[0] = (float)matpbr->GetColor()[0] * 0.003921f;
      color_float[1] = (float)matpbr->GetColor()[1] * 0.003921f;
      color_float[2] = (float)matpbr->GetColor()[2] * 0.003921f;
      color_float[3] = (float)matpbr->GetColor()[3] * 0.003921f;

      glUniform1f(metallic, matpbr->GetMetallic());
      glUniform1f(roughness, matpbr->GetRoughness());
      glUniform1f(ambient_occlusion, matpbr->GetAmbientOcclusion());

      if (matpbr->GetNormalMap() != nullptr)
      {
        glUniform1i(glGetUniformLocation(mat_->program_id_, "u_normalMap"), matpbr->GetNormalMap()->texture_id_);
      }

      if (matpbr->GetMetallicMap() != nullptr)
      {
        glUniform1i(glGetUniformLocation(mat_->program_id_, "u_metallicMap"), matpbr->GetMetallicMap()->texture_id_);
      }

      if (matpbr->GetRoughnessMap() != nullptr)
      {
        glUniform1i(glGetUniformLocation(mat_->program_id_, "u_roughnessMap"), matpbr->GetRoughnessMap()->texture_id_);
      }

      if (matpbr->GetAOMap() != nullptr)
      {
        glUniform1i(glGetUniformLocation(mat_->program_id_, "u_aoMap"), matpbr->GetAOMap()->texture_id_);
      }

      glUniform1i(glGetUniformLocation(mat_->program_id_, "u_activebloom"),
        RL.camera_->bloom_active_);

      glUniform1i(glGetUniformLocation(mat_->program_id_, "u_gPosition"), RenderList::Instance().deferred_fb_.g_position_texture_);
      glUniform1i(glGetUniformLocation(mat_->program_id_, "u_gNormal"), RenderList::Instance().deferred_fb_.g_normal_texture_);
      glUniform1i(glGetUniformLocation(mat_->program_id_, "u_gAlbedoSpec"), RenderList::Instance().deferred_fb_.g_spec_albedo_texture_);


      break;
    case Material::kMaterial_Phong:

      MaterialPhong* matphong;
      matphong = dynamic_cast<MaterialPhong*>(mat_);

      color_float[0] = (float)matphong->GetColor()[0] * 0.003921f;
      color_float[1] = (float)matphong->GetColor()[1] * 0.003921f;
      color_float[2] = (float)matphong->GetColor()[2] * 0.003921f;
      color_float[3] = (float)matphong->GetColor()[3] * 0.003921f;

      break;
    default:
      break;
    }

    if (mat_->GetTexture() != nullptr)
    {
      glUniform1i(text, mat_->GetTexture()->texture_id_);

    }



    glUniform3f(loc, 0.0f, 0.0f, 0.0f);
    glUniform4f(color, color_float[0], color_float[1], color_float[2], color_float[3]);
    glUniformMatrix4fv(mvp, 1, GL_FALSE, glm::value_ptr(mvp_matrix));
    glUniformMatrix4fv(model, 1, GL_FALSE, glm::value_ptr(transform_));
    if (!RL.lights_.light_spaces_dir_.empty())
    {
      glUniformMatrix4fv(glGetUniformLocation(mat_->program_id_, "u_lightSpaceMatrixDir"), RL.lights_.light_spaces_dir_.size(), GL_FALSE, glm::value_ptr(RL.lights_.light_spaces_dir_[0]));
      glUniform1f(glGetUniformLocation(mat_->program_id_, "u_numShadowLightsDir"), RL.lights_.light_spaces_dir_.size());
    }
    if (!RL.lights_.light_spaces_spot_.empty())
    {
      glUniformMatrix4fv(glGetUniformLocation(mat_->program_id_, "u_lightSpaceMatrixSpot"), RL.lights_.light_spaces_spot_.size(), GL_FALSE, glm::value_ptr(RL.lights_.light_spaces_spot_[0]));
      glUniform1f(glGetUniformLocation(mat_->program_id_, "u_numShadowLightsSpot"), RL.lights_.light_spaces_spot_.size());
    }


    unsigned int directional_number = RL.lights_.lightdirpos_.size();
    unsigned int point_number = RL.lights_.lightpointpos_.size();
    unsigned int spot_number = RL.lights_.lightspotpos_.size();

    unsigned int directional_number_shadow = RL.lights_.lightdirposshadow_.size();
    unsigned int point_number_shadow = RL.lights_.lightpointposshadow_.size();
    unsigned int spot_number_shadow = RL.lights_.lightspotposshadow_.size();


    // All light uniforms
    if (directional_number > 0)
    {
      glUniform1f(glGetUniformLocation(mat_->program_id_, "u_NumDirectionals"), directional_number);
      glUniform3fv(glGetUniformLocation(mat_->program_id_, "u_LightDirPosition"), directional_number, glm::value_ptr(RL.lights_.lightdirpos_[0]));
      glUniform3fv(glGetUniformLocation(mat_->program_id_, "u_LightDirDirection"), directional_number, glm::value_ptr(RL.lights_.lightdirdir_[0]));
      glUniform3fv(glGetUniformLocation(mat_->program_id_, "u_LightDirColor"), directional_number, glm::value_ptr(RL.lights_.lightdircolor_[0]));
      glUniform1fv(glGetUniformLocation(mat_->program_id_, "u_LightDirIntensity"), directional_number, &RL.lights_.lightdirintensity_[0]);
    }
    else
    {
      glUniform1f(glGetUniformLocation(mat_->program_id_, "u_NumDirectionals"), 0);
    }

    if (directional_number_shadow > 0)
    {
      glUniform1f(glGetUniformLocation(mat_->program_id_, "u_NumDirectionalsShadow"), directional_number_shadow);
      glUniform3fv(glGetUniformLocation(mat_->program_id_, "u_LightDirPositionShadows"), directional_number_shadow, glm::value_ptr(RL.lights_.lightdirposshadow_[0]));
      glUniform3fv(glGetUniformLocation(mat_->program_id_, "u_LightDirDirectionShadows"), directional_number_shadow, glm::value_ptr(RL.lights_.lightdirdirshadow_[0]));
      glUniform3fv(glGetUniformLocation(mat_->program_id_, "u_LightDirColorShadows"), directional_number_shadow, glm::value_ptr(RL.lights_.lightdircolorshadow_[0]));
      glUniform1fv(glGetUniformLocation(mat_->program_id_, "u_LightDirIntensityShadows"), directional_number_shadow, &RL.lights_.lightdirintensityshadow_[0]);
      glUniform1iv(glGetUniformLocation(mat_->program_id_, "u_shadowMapDir"), RL.lights_.shadow_map_dir_.size(), &RL.lights_.shadow_map_dir_[0]);
    }
    else
    {
      glUniform1f(glGetUniformLocation(mat_->program_id_, "u_NumDirectionalsShadow"), 0);
    }

    if (point_number > 0)
    {
      glUniform1f(glGetUniformLocation(mat_->program_id_, "u_NumPoints"), point_number);
      glUniform3fv(glGetUniformLocation(mat_->program_id_, "u_LightPointPosition"), point_number, glm::value_ptr(RL.lights_.lightpointpos_[0]));
      glUniform3fv(glGetUniformLocation(mat_->program_id_, "u_LightPointDirection"), point_number, glm::value_ptr(RL.lights_.lightpointdir_[0]));
      glUniform3fv(glGetUniformLocation(mat_->program_id_, "u_LightPointColor"), point_number, glm::value_ptr(RL.lights_.lightpointcolor_[0]));
      glUniform1fv(glGetUniformLocation(mat_->program_id_, "u_LightPointIntensity"), point_number, &RL.lights_.lightpointintensity_[0]);
    }
    else
    {
      glUniform1f(glGetUniformLocation(mat_->program_id_, "u_NumPoints"), 0);
    }

    if (spot_number > 0)
    {
      glUniform1f(glGetUniformLocation(mat_->program_id_, "u_NumSpots"), spot_number);
      glUniform3fv(glGetUniformLocation(mat_->program_id_, "u_LightSpotPosition"), spot_number, glm::value_ptr(RL.lights_.lightspotpos_[0]));
      glUniform3fv(glGetUniformLocation(mat_->program_id_, "u_LightSpotDirection"), spot_number, glm::value_ptr(RL.lights_.lightspotdir_[0]));
      glUniform3fv(glGetUniformLocation(mat_->program_id_, "u_LightSpotColor"), spot_number, glm::value_ptr(RL.lights_.lightspotcolor_[0]));
      glUniform1fv(glGetUniformLocation(mat_->program_id_, "u_LightSpotInnerAngle"), spot_number, &RL.lights_.lightspotinnerangle_[0]);
      glUniform1fv(glGetUniformLocation(mat_->program_id_, "u_LightSpotOuterAngle"), spot_number, &RL.lights_.lightspotouterangle_[0]);
      glUniform1fv(glGetUniformLocation(mat_->program_id_, "u_LightSpotIntensity"), spot_number, &RL.lights_.lightspotintensity_[0]);


    }
    else
    {
      glUniform1f(glGetUniformLocation(mat_->program_id_, "u_NumSpots"), 0);
    }

    if (spot_number_shadow > 0)
    {
      glUniform1f(glGetUniformLocation(mat_->program_id_, "u_NumSpotsShadow"), spot_number_shadow);
      glUniform3fv(glGetUniformLocation(mat_->program_id_, "u_LightSpotPositionShadows"), spot_number_shadow, glm::value_ptr(RL.lights_.lightspotposshadow_[0]));
      glUniform3fv(glGetUniformLocation(mat_->program_id_, "u_LightSpotDirectionShadows"), spot_number_shadow, glm::value_ptr(RL.lights_.lightspotdirshadow_[0]));
      glUniform3fv(glGetUniformLocation(mat_->program_id_, "u_LightSpotColorShadows"), spot_number_shadow, glm::value_ptr(RL.lights_.lightspotcolorshadow_[0]));
      glUniform1fv(glGetUniformLocation(mat_->program_id_, "u_LightSpotInnerAngleShadows"), spot_number_shadow, &RL.lights_.lightspotinnerangleshadow_[0]);
      glUniform1fv(glGetUniformLocation(mat_->program_id_, "u_LightSpotOuterAngleShadows"), spot_number_shadow, &RL.lights_.lightspotouterangleshadow_[0]);
      glUniform1fv(glGetUniformLocation(mat_->program_id_, "u_LightSpotIntensityShadows"), spot_number_shadow, &RL.lights_.lightspotintensityshadow_[0]);
      glUniform1iv(glGetUniformLocation(mat_->program_id_, "u_shadowMapSpot"), RL.lights_.shadow_map_spot_.size(), &RL.lights_.shadow_map_spot_[0]);
    }
    else
    {
      glUniform1f(glGetUniformLocation(mat_->program_id_, "u_NumSpotsShadow"), 0);
    }

    glUniform3f(glGetUniformLocation(mat_->program_id_, "u_camPos"), RL.camera_->position_.x,
      RL.camera_->position_.y,
      RL.camera_->position_.z);

  }


  glBindVertexArray(vao_);
  glDrawElements(GL_TRIANGLES, index_.size(), GL_UNSIGNED_INT, 0);

  GLenum error = glGetError();

  if (error != GL_NO_ERROR)
  {
    printf("OPENGL error");
  }
}
