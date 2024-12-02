#include "cleardrawentities.h"
#include "ecs.h"
#include "RenderList.h"

coalengine::ClearDrawEntities::ClearDrawEntities()
{
}

coalengine::ClearDrawEntities::~ClearDrawEntities()
{
}

void coalengine::ClearDrawEntities::run()
{
  ECS::Instance().entities_to_draw_.clear();

}
