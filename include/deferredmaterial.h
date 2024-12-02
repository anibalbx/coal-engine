#ifndef __DEFERRED_MATERIAL_H__
#define __DEFERRED_MATERIAL_H__ 1

#include "material.h"

namespace coalengine
{
  // Material that will store the geometry information on the G-Buffer.
  class DeferredMaterial : public Material
  {
  public:
   
    DeferredMaterial();

    DeferredMaterial(const DeferredMaterial& other) = delete;

    DeferredMaterial(DeferredMaterial&& other);

    ~DeferredMaterial();

    DeferredMaterial& operator= (const DeferredMaterial& other) = delete;

    DeferredMaterial& operator= (DeferredMaterial&& other);
  };
}

#endif
