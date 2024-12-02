#ifndef __MATERIAL_PHONG_H__
#define __MATERIAL_PHONG_H__ 1

#include "material.h"


namespace coalengine
{
  class MaterialPhong : public Material
  {
   public:
    MaterialPhong();
    MaterialPhong(const MaterialPhong& other) = delete;
    MaterialPhong(MaterialPhong&& other);
    ~MaterialPhong();
    MaterialPhong& operator= (const MaterialPhong& other) = delete;
    MaterialPhong& operator= (MaterialPhong&& other);
  };
}

#endif
