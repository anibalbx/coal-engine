#ifndef __WORLDTRANSFORM_H__
#define __WORLDTRANSFORM_H__ 1

#include "glm/glm.hpp"

namespace coalengine
{
  class WorldTransform
  {
  public:
    WorldTransform();
    ~WorldTransform();
    WorldTransform(const WorldTransform& o) = delete;
    WorldTransform(WorldTransform&& o);
    WorldTransform& operator= (const WorldTransform& o) = delete;
    WorldTransform& operator= (WorldTransform&& o);

    void SetWorld(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale);

    glm::vec3 position_;
    glm::vec3 rotation_;
    glm::vec3 scale_;

    glm::mat4 world_;   
  };
}


#endif

