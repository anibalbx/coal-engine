#include "localtransform.h"
#include "entity.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "ecs.h"
#include "worldtransform.h"

coalengine::LocalTransform::LocalTransform()
{
  local_ = glm::mat4(1.0);
  position_ = glm::vec3(0.0f);
  rotation_ = glm::vec3(0.0f);
  scale_ = glm::vec3(1.0f);
  parent_ = nullptr;
}

coalengine::LocalTransform::~LocalTransform()
{
}

coalengine::LocalTransform::LocalTransform(LocalTransform&& o)
{
  local_ = o.local_;
  parent_ = o.parent_;
}

coalengine::LocalTransform& coalengine::LocalTransform::operator=(LocalTransform&& o)
{
  local_ = o.local_;
  parent_ = o.parent_;
  return  *this;
}

void coalengine::LocalTransform::SetParent(Entity* parent)
{
  if(parent == nullptr)
  {
    parent_ = nullptr;
  }
  else
  {
    if (parent->id_ != 0 && parent->HasComponents(Entity::WORLD))
    {
      parent_ = parent;
    }
  }
}

coalengine::Entity* coalengine::LocalTransform::GetParent()
{
  return parent_;
}

void coalengine::LocalTransform::SetLocal(glm::vec3 pos, glm::vec3 rot, glm::vec3 scale)
{
  // Generate the model matrix
  glm::mat4 scalingMatrix = glm::scale(glm::mat4(1.0f), scale);

  glm::mat4 rotationMatrixX = glm::rotate(glm::mat4(1.0f), glm::radians(rot.x), glm::vec3(1.0f, 0.0f, 0.0f));
  glm::mat4 rotationMatrixY = glm::rotate(glm::mat4(1.0f), glm::radians(rot.y), glm::vec3(0.0f, 1.0f, 0.0f));
  glm::mat4 rotationMatrixZ = glm::rotate(glm::mat4(1.0f), glm::radians(rot.z), glm::vec3(0.0f, 0.0f, 1.0f));

  glm::mat4 translationMatrix = glm::translate((glm::mat4(1.0f), pos));

  glm::mat4 model = translationMatrix * rotationMatrixX * rotationMatrixY * rotationMatrixZ * scalingMatrix;

  local_ = model;
  position_ = pos;
  rotation_ = rot;
  scale_ = scale; 
}
