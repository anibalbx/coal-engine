#ifndef __LOCALTRANSFORMTRS_H__
#define __LOCALTRANSFORMTRS_H__ 1

#include "glm/glm.hpp"

namespace coalengine
{
  class LocalTransformTRS
  {
  public:
    LocalTransformTRS();

    ~LocalTransformTRS();

    LocalTransformTRS(const LocalTransformTRS& o) = delete;

    LocalTransformTRS(LocalTransformTRS&& o);

    LocalTransformTRS& operator= (const LocalTransformTRS& o) = delete;

    LocalTransformTRS& operator= (LocalTransformTRS&& o);

	  void SetPosition(glm::vec3 position);
	  void SetPosition(float x, float y, float z);

	  void SetRotation(glm::vec3 rotation);
	  void SetRotation(float x, float y, float z);

	  void SetScale(glm::vec3 scale);
	  void SetScale(float x, float y, float z);
    
	  glm::vec3 GetPosition();
	  glm::vec3 GetRotation();
	  glm::vec3 GetScale();

    glm::vec3 position_;
    glm::vec3 rotation_;
    glm::vec3 scale_;

  };
}

#endif
