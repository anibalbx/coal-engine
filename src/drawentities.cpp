#include "drawentities.h"

#include "ecs.h"
#include "RenderList.h"

coalengine::DrawEntities::DrawEntities()
{
}

coalengine::DrawEntities::~DrawEntities()
{
}

void coalengine::DrawEntities::run()
{

  // Loop all the entities on scene (entities with render component)
  for (unsigned int i = 0; i < ECS::Instance().entities_to_draw_.size(); ++i)
  {
    Entity* ent = ECS::Instance().entities_to_draw_.at(i);

    // Get the world component to access the model matrix
    WorldTransform* worldComp = ECS::Instance().GetWorldTransform(ent);

    // Get the mesh
    RenderComponent* renderComp = ECS::Instance().GetRenderComponent(ent);

    // Not render during shadow pass
    if (RenderList::Instance().state_ == RenderList::kShadowRenderST)
    {
      if (renderComp->GetMesh() != nullptr && worldComp != nullptr)
      {
        
        renderComp->GetMesh()->transform_ = worldComp->world_;
        if(renderComp->b_throw_shadows)
        {
          renderComp->GetMesh()->RenderGeometry();
        }       
      }
    }
    else
    {
      if (renderComp->GetMesh() != nullptr && worldComp != nullptr)
      {
        renderComp->GetMesh()->transform_ = worldComp->world_;
        renderComp->GetMesh()->RenderGeometry();
      }
    }
  }
}
