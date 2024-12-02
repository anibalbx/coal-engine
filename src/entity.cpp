#include "entity.h"

#include "ecs.h"

coalengine::Entity::Entity()
{
  id_ = 0;
  components_ = 0;
  uploaded_ = false;
  newcomp_ = false;
  name_ = "Entity";

}

coalengine::Entity::~Entity()
{
}

coalengine::Entity::Entity(Entity&& o)
{
  id_ = o.id_;
  components_ = o.components_;
  name_ = o.name_;
}

coalengine::Entity& coalengine::Entity::operator=(Entity&& o)
{
  id_ = o.id_;
  components_ = o.components_;
  name_ = o.name_;
  return *this;
}

bool coalengine::Entity::MatchesComponents(uint32 mask)
{
  // OR 
  mask = ~mask;

  // AND to check if matches the components
  if ((components_ & mask) == 0)
  {
    return true;
  }

  return false;
}

bool coalengine::Entity::HasComponents(uint32 mask)
{
  // AND to check if the mask has the given components
  if ((components_ & mask) == mask)
  {
    return true;
  }

  return false;
}

void coalengine::Entity::SetChildren(Entity* ent)
{
  if (ent != nullptr && ent->id_ != 0 && HasComponents(WORLD))
  {
    if (ent->HasComponents(WORLD | LOCAL))
    {
      LocalTransform* local_c = ECS::Instance().GetLocalTransform(ent);
      local_c->SetParent(this);
      childs_.insert(std::make_pair(ECS::Instance().RealEntityId(ent->id_), ent));
    }
  }
}

void coalengine::Entity::SetName(std::string name)
{
  name_ = std::move(name);
}
