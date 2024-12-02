#ifndef __MATERIAL_PBR_H__
#define __MATERIAL_PBR_H__ 1

#include "material.h"

namespace coalengine
{

  class MaterialPBR : public Material
  {
  public:

    MaterialPBR();

    MaterialPBR(const MaterialPBR& other) = delete;

    MaterialPBR(MaterialPBR&& other);

    ~MaterialPBR();

    MaterialPBR& operator= (const MaterialPBR& other) = delete;

    MaterialPBR& operator= (MaterialPBR&& other);

    void SetNormalMap(uint32 texture) override;
    Texture* GetNormalMap() override;

    void SetMetallicMap(uint32 texture) override;
    Texture* GetMetallicMap() override;

    void SetRoughnessMap(uint32 texture) override;
    Texture* GetRoughnessMap() override;

    void SetAOMap(uint32 texture) override;
    Texture* GetAOMap() override;

    void SetMetallic(float metallic) override;
    float GetMetallic() override;

    void SetRoughness(float roughness) override;
    float GetRoughness() override;

    void SetAmbientOcclusion(float ao) override;
    float GetAmbientOcclusion() override;

  private:

    Texture* normal_map;
    Texture* metallic_map;
    Texture* roughness_map;
    Texture* ao_map;

    float metallic_;
    float roughness_;
    float ao_;
  };
}

#endif


