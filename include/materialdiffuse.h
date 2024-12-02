#ifndef __MATERIAL_DIFFUSE_H__
#define __MATERIAL_DIFFUSE_H__ 1

#include "material.h"

namespace coalengine
{


  class MaterialDiffuse : public Material
  {
  public:

    MaterialDiffuse();

    MaterialDiffuse(const MaterialDiffuse& other) = delete;

    MaterialDiffuse(MaterialDiffuse&& other);

    ~MaterialDiffuse();

    MaterialDiffuse& operator= (const MaterialDiffuse& other) = delete;

    MaterialDiffuse& operator= (MaterialDiffuse&& other);

    void SetIsEmissive(bool value) override;

    bool GetIsEmissive() override;

    bool is_emissive_;

  };
}

#endif



