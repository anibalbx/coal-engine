#include "localtransformtrs.h"

coalengine::LocalTransformTRS::LocalTransformTRS()
{
  position_ = glm::vec3(0.0);
  rotation_ = glm::vec3(0.0);
  scale_ = glm::vec3(1.0);
}

coalengine::LocalTransformTRS::~LocalTransformTRS()
{
}

coalengine::LocalTransformTRS::LocalTransformTRS(LocalTransformTRS&& o)
{
  position_ = o.position_;
  rotation_ = o.rotation_;
  scale_ = o.scale_;
}

coalengine::LocalTransformTRS& coalengine::LocalTransformTRS::operator=(LocalTransformTRS&& o)
{
  position_ = o.position_;
  rotation_ = o.rotation_;
  scale_ = o.scale_;
  return *this;
}

void coalengine::LocalTransformTRS::SetPosition(glm::vec3 position)
{
	position_ = position;
}

void coalengine::LocalTransformTRS::SetPosition(float x, float y, float z)
{
	position_.x = x;
	position_.y = y;
	position_.z = z;
}

void coalengine::LocalTransformTRS::SetRotation(glm::vec3 rotation)
{
	rotation_ = rotation;
}

void coalengine::LocalTransformTRS::SetRotation(float x, float y, float z)
{
	rotation_.x = x;
	rotation_.y = y;
	rotation_.z = z;
}

void coalengine::LocalTransformTRS::SetScale(glm::vec3 scale)
{
	scale_ = scale;
}

void coalengine::LocalTransformTRS::SetScale(float x, float y, float z)
{
	scale_.x = x;
	scale_.y = y;
	scale_.z = z;
}

glm::vec3 coalengine::LocalTransformTRS::GetPosition()
{
	return position_;
}

glm::vec3 coalengine::LocalTransformTRS::GetRotation()
{
	return rotation_;
}

glm::vec3 coalengine::LocalTransformTRS::GetScale()
{
	return scale_;
}
