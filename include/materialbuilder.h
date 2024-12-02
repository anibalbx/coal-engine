#ifndef __MATERIAL_BUILDER_H__
#define __MATERIAL_BUILDER_H__ 1

#include "types.h"
#include "material.h"

namespace coalengine
{
  class MaterialBuilder
  {
  public:
    MaterialBuilder();
    ~MaterialBuilder();
    MaterialBuilder(const MaterialBuilder& o) = delete;
    MaterialBuilder(MaterialBuilder&& o);

    MaterialBuilder& operator=(const MaterialBuilder& o) = delete;
    MaterialBuilder& operator=(MaterialBuilder&& o);

    void CreateMaterial(Material::Type type);

    void SetColor(glm::vec4 color);

    void SetColor(uint32 material, glm::vec4 color);

    void SetColor(float r, float g, float b, float a);

    void SetColor(uint32 material, float r, float g,
      float b, float a);

    void SetEmissive(uint32 material, bool value);

    bool GetEmissive(uint32 material);

    void SetTexture(uint32 texture);

    void SetTexture(uint32 material, uint32 texture);

    glm::vec4 GetColor(uint32 material);

    uint32 GetTexture(uint32 material);

    static bool IsValidMaterial(uint32 id);

    uint32 Finish();

    void SetNormalMap(uint32 texture);
    void SetNormalMap(uint32 material, uint32 texture);
    uint32 GetNormalMap(uint32 material);

    void SetMetallicMap(uint32 texture);
    void SetMetallicMap(uint32 material, uint32 texture);
    uint32 GetMetallicMap(uint32 material);

    void SetRoughnessMap(uint32 texture);
    void SetRoughnessMap(uint32 material, uint32 texture);
    uint32 GetRoughnessMap(uint32 material);

    void SetAOMap(uint32 texture);
    void SetAOMap(uint32 material, uint32 texture);
    uint32 GetAOMap(uint32 material);

    void SetMetallic(uint32 material, float metallic);
    void SetMetallic(float metallic);
    float GetMetallic(uint32 material);

    void SetRoughness(uint32 material, float roughness);
    void SetRoughness(float roughness);
    float GetRoughness(uint32 material);

    void SetAmbientOcclusion(uint32 material, float ao);
    void SetAmbientOcclusion(float ao);
    float GetAmbientOcclusion(uint32 material);

    glm::vec4 color_;
    float metallic_;
    float roughness_;
    float ambient_occlusion_;
    Material::Type type_;
    uint32 texture_id_;
    uint32 normal_map_id_;
    uint32 metallic_map_id_;
    uint32 roughness_map_id_;
    uint32 ao_map_id_;

  private:

    // Reset an clear all builder parameters.
    void ResetBuilder();
  };
}

#endif