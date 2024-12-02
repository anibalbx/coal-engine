#include "materialphong.h"
#include "engineutils.h"
#include "RenderList.h"

coalengine::MaterialPhong::MaterialPhong()
{
  EngineUtils::Instance().LoadStringFromFile("../../../data/Shaders/Phong.vert", vertex_source_);
  EngineUtils::Instance().LoadStringFromFile("../../../data/Shaders/Phong.frag", fragment_source_);
}

coalengine::MaterialPhong::MaterialPhong(MaterialPhong&& other) : Material(std::move(other))
{
}

coalengine::MaterialPhong::~MaterialPhong()
{
}

coalengine::MaterialPhong& coalengine::MaterialPhong::operator=(MaterialPhong&& other)
{
  Material::operator=(std::move(other));
  return *this;
}
