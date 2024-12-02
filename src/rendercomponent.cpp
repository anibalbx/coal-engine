#include "rendercomponent.h"


#include "engineutils.h"
#include "RenderList.h"

coalengine::RenderComponent::RenderComponent()
{
  mesh_ = nullptr;
  b_throw_shadows = true;
  box = BoundingBox();
}

coalengine::RenderComponent::RenderComponent(RenderComponent&& o)
{
  mesh_ = o.mesh_;
  b_throw_shadows = o.b_throw_shadows;
  box = o.box;
}

coalengine::RenderComponent::~RenderComponent()
{
}

coalengine::RenderComponent& coalengine::RenderComponent::operator=(RenderComponent&& o)
{
  mesh_ = o.mesh_;
  b_throw_shadows = o.b_throw_shadows;
  box = o.box;
  return  *this;
}

void coalengine::RenderComponent::SetMesh(uint32 geometry)
{
  if (RenderList::Instance().IsValidGeometry(geometry))
  {
    mesh_ = RenderList::Instance().geometry_map_[geometry].get();
    box = EngineUtils::Instance().GenerateBoundingBox(mesh_->vertices_);
  }
}

void coalengine::RenderComponent::QuitMesh()
{
  mesh_ = nullptr;
}

coalengine::Geometry* coalengine::RenderComponent::GetMesh()
{
  return mesh_;
}
