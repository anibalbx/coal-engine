#include "materialpbr.h"
#include "engineutils.h"
#include "renderlist.h"
#include "texturebuilder.h"
#include "texture.h"

coalengine::MaterialPBR::MaterialPBR()
{
  mat_type_ = kMaterial_PBR;
  normal_map = nullptr;
  metallic_map = nullptr;
  roughness_map = nullptr;
  ao_map = nullptr;
  EngineUtils::Instance().LoadStringFromFile("../../../data/Shaders/PBR.vert", vertex_source_);
  EngineUtils::Instance().LoadStringFromFile("../../../data/Shaders/PBR.frag", fragment_source_);
  metallic_ = 0.0f;
  roughness_ = 0.0f;
  ao_ = 0.0f;
}

coalengine::MaterialPBR::MaterialPBR(MaterialPBR&& other) : Material(std::move(other))
{
  normal_map = other.normal_map;
  metallic_map = other.metallic_map;
  roughness_map = other.roughness_map;
  ao_map = other.ao_map;
  metallic_ = other.metallic_;
  roughness_ = other.roughness_;
  ao_ = other.ao_;
}

coalengine::MaterialPBR::~MaterialPBR()
{
}

coalengine::MaterialPBR& coalengine::MaterialPBR::operator=(MaterialPBR&& other)
{
  Material::operator=(std::move(other));
  normal_map = other.normal_map;
  metallic_map = other.metallic_map;
  roughness_map = other.roughness_map;
  ao_map = other.ao_map;
  metallic_ = other.metallic_;
  roughness_ = other.roughness_;
  ao_ = other.ao_;
  return *this;
}


void coalengine::MaterialPBR::SetNormalMap(uint32 texture)
{
  if (TextureBuilder::IsValidTexture(texture))
  {
    normal_map = RenderList::Instance().texture_map_[texture].get();
  }
}

coalengine::Texture* coalengine::MaterialPBR::GetNormalMap()
{
  return normal_map;
}

void coalengine::MaterialPBR::SetMetallicMap(uint32 texture)
{
  if (TextureBuilder::IsValidTexture(texture))
  {
    metallic_map = RenderList::Instance().texture_map_[texture].get();
  }
}

coalengine::Texture* coalengine::MaterialPBR::GetMetallicMap()
{
  return metallic_map;
}

void coalengine::MaterialPBR::SetRoughnessMap(uint32 texture)
{
  if (TextureBuilder::IsValidTexture(texture))
  {
    roughness_map = RenderList::Instance().texture_map_[texture].get();
  }
}

coalengine::Texture* coalengine::MaterialPBR::GetRoughnessMap()
{
  return roughness_map;
}

void coalengine::MaterialPBR::SetAOMap(uint32 texture)
{
  if (TextureBuilder::IsValidTexture(texture))
  {
    ao_map = RenderList::Instance().texture_map_[texture].get();
  }
}

coalengine::Texture* coalengine::MaterialPBR::GetAOMap()
{
  return ao_map;
}

void coalengine::MaterialPBR::SetMetallic(float metallic)
{
  metallic_ = metallic;
}

float coalengine::MaterialPBR::GetMetallic()
{
  return metallic_;
}

void coalengine::MaterialPBR::SetRoughness(float roughness)
{
  roughness_ = roughness;
}

float coalengine::MaterialPBR::GetRoughness()
{
  return roughness_;
}

void coalengine::MaterialPBR::SetAmbientOcclusion(float ao)
{
  ao_ = ao;
}

float coalengine::MaterialPBR::GetAmbientOcclusion()
{
  return ao_;
}
