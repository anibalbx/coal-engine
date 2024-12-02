#ifndef __LOCALTRANSFORM_H__
#define __LOCALTRANSFORM_H__ 1

#include "glm/glm.hpp"

namespace coalengine
{
  class Entity;

  class LocalTransform
  {
  public:
    LocalTransform();
    ~LocalTransform();
    LocalTransform(const LocalTransform& o) = delete;
    LocalTransform(LocalTransform&& o);
    LocalTransform& operator= (const LocalTransform& o) = delete;
    LocalTransform& operator= (LocalTransform&& o);

    void SetParent(Entity* parent);
    Entity* GetParent();

    // Set local matrix respect from the parent.
    void SetLocal(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale);

    glm::mat4 local_;

    glm::vec3 position_;
    glm::vec3 rotation_;
    glm::vec3 scale_;
   
  private:
    //pointer to ent parent
    coalengine::Entity* parent_;
  };

}

#endif
