#include "materialbuilder.h"

#include <px_mem.h>

#include "deferredmaterial.h"
#include "materialdiffuse.h"
#include "RenderList.h"
#include "texture.h"
#include "materialpbr.h"
#include "materialphong.h"
#include "texturebuilder.h"
#include "shadowmaterial.h"

coalengine::MaterialBuilder::MaterialBuilder()
{
  color_[0] = 0;
  color_[1] = 0;
  color_[2] = 0;
  color_[3] = 0;
  texture_id_ = 0;
  normal_map_id_ = 0;
  metallic_map_id_ = 0;
  roughness_map_id_ = 0;
  ao_map_id_ = 0;
  metallic_ = 0.0f;
  roughness_ = 0.0f;
  ambient_occlusion_ = 0.0f;
  type_ = Material::Type::kNone;
}

coalengine::MaterialBuilder::~MaterialBuilder()
{
}

coalengine::MaterialBuilder::MaterialBuilder(MaterialBuilder&& o)
{
  color_[0] = o.color_[0];
  color_[1] = o.color_[1];
  color_[2] = o.color_[2];
  color_[3] = o.color_[3];

  metallic_ = o.metallic_;
  roughness_ = o.roughness_;
  ambient_occlusion_ = 0.0f;

  type_ = o.type_;
  texture_id_ = o.texture_id_;
  normal_map_id_ = o.normal_map_id_;
  metallic_map_id_ = o.metallic_map_id_;
  roughness_map_id_ = o.roughness_map_id_;
  ao_map_id_ = o.ao_map_id_;
}

coalengine::MaterialBuilder& coalengine::MaterialBuilder::operator=(MaterialBuilder&& o)
{
  color_[0] = o.color_[0];
  color_[1] = o.color_[1];
  color_[2] = o.color_[2];
  color_[3] = o.color_[3];

  metallic_ = o.metallic_;
  roughness_ = o.roughness_;
  ambient_occlusion_ = 0.0f;

  type_ = o.type_;
  texture_id_ = o.texture_id_;
  normal_map_id_ = o.normal_map_id_;
  metallic_map_id_ = o.metallic_map_id_;
  roughness_map_id_ = o.roughness_map_id_;
  ao_map_id_ = o.ao_map_id_;
  return  *this;
}

void coalengine::MaterialBuilder::CreateMaterial(Material::Type type)
{
  type_ = type;
}

void coalengine::MaterialBuilder::SetColor(glm::vec4 color)
{
  color_.x = color.x;
  color_.y = color.y;
  color_.z = color.z;
  color_.w = color.w;
}

void coalengine::MaterialBuilder::SetColor(uint32 material, glm::vec4 color)
{
  if (IsValidMaterial(material))
  {
    RenderList::Instance().material_map_[material]->SetColor(color);
  }

}

void coalengine::MaterialBuilder::SetColor(float r, float g, float b, float a)
{
  color_.x = r;
  color_.y = g;
  color_.z = b;
  color_.w = a;
}

void coalengine::MaterialBuilder::SetColor(uint32 material, float r, float g, float b, float a)
{
  glm::vec4 color = glm::vec4(r, g, b, a);
  if (IsValidMaterial(material))
  {
    RenderList::Instance().material_map_[material]->SetColor(color);
  }
}

void coalengine::MaterialBuilder::SetEmissive(uint32 material, bool value)
{
  if (IsValidMaterial(material))
  {
    RenderList::Instance().material_map_[material]->SetIsEmissive(value);
  }
}

bool coalengine::MaterialBuilder::GetEmissive(uint32 material)
{
  if (IsValidMaterial(material))
  {
    return RenderList::Instance().material_map_[material]->GetIsEmissive();
  }

  return false;
}

void coalengine::MaterialBuilder::SetTexture(uint32 texture)
{
  if (TextureBuilder::IsValidTexture(texture))
  {
    texture_id_ = texture;
  }
}

void coalengine::MaterialBuilder::SetTexture(uint32 material, uint32 texture)
{
  if (IsValidMaterial(material))
  {
    Material* mat = RenderList::Instance().material_map_[material].get();

    if (mat != nullptr)
    {
      mat->SetTexture(texture);
    }
  }
}

glm::vec4 coalengine::MaterialBuilder::GetColor(uint32 material)
{
  if (IsValidMaterial(material))
  {
    return RenderList::Instance().material_map_[material]->GetColor();
  }

  return glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
}

uint32 coalengine::MaterialBuilder::GetTexture(uint32 material)
{
  if (IsValidMaterial(material))
  {
    return RenderList::Instance().material_map_[material]->GetTexture()->id_;
  }

  return 0;
}

bool coalengine::MaterialBuilder::IsValidMaterial(uint32 id)
{
  if (RenderList::Instance().IsMaterial(id))
  {
    std::map<uint32, px::Mem<Material>>::iterator it =
      RenderList::Instance().material_map_.find(id);

    if (it != RenderList::Instance().material_map_.end())
    {
      return true;
    }

  }
  return false;
}

uint32 coalengine::MaterialBuilder::Finish()
{
  px::Mem<Material> mat;

  switch (type_) {
  case Material::kNone:
    return 0;
  case Material::kMaterial_Diffuse:
    mat.alloc<MaterialDiffuse>();
    break;
  case Material::kMaterial_PBR:
    mat.alloc<MaterialPBR>();
    break;
  case Material::kMaterial_Shadow:
    mat.alloc<ShadowMaterial>();
    break;
  case Material::kMaterial_Phong:
    mat.alloc<MaterialPhong>();
    break;
  default:
    return 0;
  case Material::kMaterial_Deferred:
    mat.alloc<DeferredMaterial>();
    break;
  }

  mat->SetColor(color_);
  mat->SetTexture(texture_id_);
  mat->SetMetallic(metallic_);
  mat->SetRoughness(roughness_);
  mat->SetAmbientOcclusion(ambient_occlusion_);
  mat->SetMetallicMap(metallic_map_id_);
  mat->SetNormalMap(normal_map_id_);
  mat->SetRoughnessMap(roughness_map_id_);
  mat->SetAOMap(ao_map_id_);

  ResetBuilder();

  uint32 id = RenderList::Instance().num_materials_;
  RenderList::Instance().num_materials_++;
  id = id | 0x30000000;
  mat->id_ = id;
  RenderList::Instance().SubmitMaterial(std::move(mat));
  return id;
}

void coalengine::MaterialBuilder::SetNormalMap(uint32 texture)
{
  if (TextureBuilder::IsValidTexture(texture))
  {

    normal_map_id_ = texture;
  }
}

void coalengine::MaterialBuilder::SetNormalMap(uint32 material, uint32 texture)
{
  if (IsValidMaterial(material))
  {
    Material* mat = RenderList::Instance().material_map_[material].get();

    if (mat != nullptr)
    {
      mat->SetNormalMap(texture);
    }
  }
}

uint32 coalengine::MaterialBuilder::GetNormalMap(uint32 material)
{
  if (IsValidMaterial(material))
  {
    return RenderList::Instance().material_map_[material]->GetNormalMap()->id_;
  }

  return 0;
}

void coalengine::MaterialBuilder::SetMetallicMap(uint32 texture)
{
  if (TextureBuilder::IsValidTexture(texture))
  {
    metallic_map_id_ = texture;
  }
}

void coalengine::MaterialBuilder::SetMetallicMap(uint32 material, uint32 texture)
{
  if (IsValidMaterial(material))
  {
    Material* mat = RenderList::Instance().material_map_[material].get();

    if (mat != nullptr)
    {
      mat->SetMetallicMap(texture);
    }
  }
}

uint32 coalengine::MaterialBuilder::GetMetallicMap(uint32 material)
{
  if (IsValidMaterial(material))
  {
    return RenderList::Instance().material_map_[material]->GetMetallicMap()->id_;
  }

  return 0;
}

void coalengine::MaterialBuilder::SetRoughnessMap(uint32 texture)
{
  if (TextureBuilder::IsValidTexture(texture))
  {
    roughness_map_id_ = texture;
  }
}

void coalengine::MaterialBuilder::SetRoughnessMap(uint32 material, uint32 texture)
{
  if (IsValidMaterial(material))
  {
    Material* mat = RenderList::Instance().material_map_[material].get();

    if (mat != nullptr)
    {
      mat->SetRoughnessMap(texture);
    }
  }
}

uint32 coalengine::MaterialBuilder::GetRoughnessMap(uint32 material)
{
  if (IsValidMaterial(material))
  {
    return RenderList::Instance().material_map_[material]->GetRoughnessMap()->id_;
  }

  return 0;
}

void coalengine::MaterialBuilder::SetAOMap(uint32 texture)
{
  if (TextureBuilder::IsValidTexture(texture))
  {
    ao_map_id_ = texture;
  }
}

void coalengine::MaterialBuilder::SetAOMap(uint32 material, uint32 texture)
{
  if (IsValidMaterial(material))
  {
    Material* mat = RenderList::Instance().material_map_[material].get();

    if (mat != nullptr)
    {
      mat->SetAOMap(texture);
    }
  }
}

uint32 coalengine::MaterialBuilder::GetAOMap(uint32 material)
{
  if (IsValidMaterial(material))
  {
    return RenderList::Instance().material_map_[material]->GetAOMap()->id_;
  }

  return 0;
}

void coalengine::MaterialBuilder::SetMetallic(uint32 material, float metallic)
{
  if (IsValidMaterial(material))
  {
    if (RenderList::Instance().material_map_[material]->mat_type_ == Material::kMaterial_PBR)
    {
      RenderList::Instance().material_map_[material]->SetMetallic(metallic);
    }
  }
}

void coalengine::MaterialBuilder::SetMetallic(float metallic)
{
  metallic_ = metallic;
}

float coalengine::MaterialBuilder::GetMetallic(uint32 material)
{
  if (IsValidMaterial(material))
  {
    if (RenderList::Instance().material_map_[material]->mat_type_ == Material::kMaterial_PBR)
    {
      return RenderList::Instance().material_map_[material]->GetMetallic();
    }
    return 0.0f;
  }

  return 0.0f;
}

void coalengine::MaterialBuilder::SetRoughness(uint32 material, float roughness)
{
  if (IsValidMaterial(material))
  {
    if (RenderList::Instance().material_map_[material]->mat_type_ == Material::kMaterial_PBR)
    {
      RenderList::Instance().material_map_[material]->SetRoughness(roughness);
    }
  }
}

void coalengine::MaterialBuilder::SetRoughness(float roughness)
{
  roughness_ = roughness;
}

float coalengine::MaterialBuilder::GetRoughness(uint32 material)
{
  if (IsValidMaterial(material))
  {
    if (RenderList::Instance().material_map_[material]->mat_type_ == Material::kMaterial_PBR)
    {
      return RenderList::Instance().material_map_[material]->GetRoughness();
    }
    return 0.0f;
  }

  return 0.0f;
}

void coalengine::MaterialBuilder::SetAmbientOcclusion(uint32 material, float ao)
{

  if (IsValidMaterial(material))
  {
    if (RenderList::Instance().material_map_[material]->mat_type_ == Material::kMaterial_PBR)
    {
      RenderList::Instance().material_map_[material]->SetAmbientOcclusion(ao);
    }
  }
}

void coalengine::MaterialBuilder::SetAmbientOcclusion(float ao)
{
  ambient_occlusion_ = ao;
}

float coalengine::MaterialBuilder::GetAmbientOcclusion(uint32 material)
{
  if (IsValidMaterial(material))
  {
    if (RenderList::Instance().material_map_[material]->mat_type_ == Material::kMaterial_PBR)
    {
      return RenderList::Instance().material_map_[material]->GetAmbientOcclusion();
    }
    return 0.0f;
  }

  return 0.0f;
}

void coalengine::MaterialBuilder::ResetBuilder()
{

  color_[0] = 0;
  color_[1] = 0;
  color_[2] = 0;
  color_[3] = 0;
  texture_id_ = 0;
  normal_map_id_ = 0;
  metallic_map_id_ = 0;
  roughness_map_id_ = 0;
  ao_map_id_ = 0;
  metallic_ = 0.0f;
  roughness_ = 0.0f;
  ambient_occlusion_ = 0.0f;
  type_ = Material::Type::kNone;
}
