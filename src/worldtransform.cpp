#include "worldtransform.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>


coalengine::WorldTransform::WorldTransform()
{
  world_ = glm::mat4(1.0);
  position_ = glm::vec3(0.0);
  rotation_ = glm::vec3(0.0);
  scale_ = glm::vec3(1.0);

}

coalengine::WorldTransform::~WorldTransform()
{
}

coalengine::WorldTransform::WorldTransform(WorldTransform&& o)
{
  world_ = o.world_;

}

coalengine::WorldTransform& coalengine::WorldTransform::operator=(WorldTransform&& o)
{
  world_ = o.world_;

  return *this;
}

void coalengine::WorldTransform::SetWorld(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale)
{
  glm::mat4 scalingMatrix = glm::scale(glm::mat4(1.0f), scale);

  glm::mat4 rotationMatrixX = glm::rotate(glm::mat4(1.0f), glm::radians(rot.x), glm::vec3(1.0f, 0.0f, 0.0f));
  glm::mat4 rotationMatrixY = glm::rotate(glm::mat4(1.0f), glm::radians(rot.y), glm::vec3(0.0f, 1.0f, 0.0f));
  glm::mat4 rotationMatrixZ = glm::rotate(glm::mat4(1.0f), glm::radians(rot.z), glm::vec3(0.0f, 0.0f, 1.0f));


  glm::mat4 translationMatrix = glm::translate((glm::mat4(1.0f), pos));

  glm::mat4 model = translationMatrix * rotationMatrixX * rotationMatrixY * rotationMatrixZ * scalingMatrix;

  world_ = model;

  position_ = pos;
  rotation_ = rot;
  scale_ = scale;

}



