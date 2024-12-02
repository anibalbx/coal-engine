#ifndef __ENTITY_H__
#define __ENTITY_H__ 1

#include <map>
#include <string>
#include <vec3.hpp>
#include "types.h"
namespace coalengine
{
  class Entity
  {
  public:
    Entity();
    ~Entity();
    Entity(const Entity& o) = delete;
    Entity(Entity&& o);
    Entity& operator= (const Entity& o) = delete;
    Entity& operator= (Entity&& o);

    // components enum that later will be used for masking a register to check which components an entity has.
    enum ECSComponents
    {
      WORLD = 1 << 0,
      LOCAL = 1 << 1,
      LOCALTRS = 1 << 2,
      RENDER = 1 << 3,
      LIGHT = 1 << 4,

    };

    uint32 id_;

    // Current entities components
    uint32 components_;

    bool uploaded_;

    std::map<uint32, Entity*> childs_;
    std::string name_;

    bool newcomp_;

    // Given a mask of components, returns true if the entity has ALL the components in the mask, returns false if not.
    bool MatchesComponents(uint32 mask);

    // Given a mask of components, returns true if the entity has ALL
    // the components in the mask, return false if some component is missing.
    // The entity might have more components tha the passed on the mask, but still returns true if has all the ones in the mask.
    bool HasComponents(uint32 mask);

    void SetChildren(Entity* ent);

    void SetName(std::string name);

  };

}

#endif
