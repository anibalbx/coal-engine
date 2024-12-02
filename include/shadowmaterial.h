#ifndef __SHADOWMATERIAL_H__
#define __SHADOWMATERIAL_H__ 1

#include "material.h"

namespace coalengine
{

  class ShadowMaterial : public Material
  {
  public:

    ShadowMaterial();

    ShadowMaterial(const ShadowMaterial& other) = delete;

    ShadowMaterial(ShadowMaterial&& other);

    ~ShadowMaterial();

    ShadowMaterial& operator= (const ShadowMaterial& other) = delete;

    ShadowMaterial& operator= (ShadowMaterial&& other);

  };
}

#endif



