#include "lightcomponent.h"

coalengine::LightComponent::LightComponent()
{
  type_ = kNone;
  position_ = glm::vec3(0.0f, 0.0f, 0.0f);
  direction_ = glm::vec3(0.0f, 0.0f, 0.0f);
  color_.x = 0.0f;
  color_.y = 0.0f;
  color_.z = 0.0f;
  color_.w = 0.0f;
  intensity_ = 0.0f;
  inner_angle_ = 0.0f;
  outer_angle_ = 0.0f;
  light_view_ = glm::mat4(0.0f);
  light_space_ = glm::mat4(0.0f);
  model_ = glm::mat4(0.0f);
  projection_ = glm::mat4(0.0f);
  bcancastshadow_ = false;

}

coalengine::LightComponent::LightComponent(LightComponent&& o)
{
  type_ = o.type_;
  position_ = o.position_;
  direction_ = o.direction_;
  color_[0] = o.color_[0];
  color_[1] = o.color_[1];
  color_[2] = o.color_[2];
  intensity_ = o.intensity_;
  inner_angle_ = 0.0f;
  outer_angle_ = 0.0f;
  light_view_ = o.light_view_;
  light_space_ = o.light_space_;
  bcancastshadow_ = o.bcancastshadow_;
}

coalengine::LightComponent::~LightComponent()
{
}

coalengine::LightComponent& coalengine::LightComponent::operator=(LightComponent&& o)
{
  type_ = o.type_;
  position_ = o.position_;
  direction_ = o.direction_;
  color_[0] = o.color_[0];
  color_[1] = o.color_[1];
  color_[2] = o.color_[2];
  intensity_ = o.intensity_;
  inner_angle_ = 0.0f;
  outer_angle_ = 0.0f;
  light_view_ = o.light_view_;
  light_space_ = o.light_space_;
  bcancastshadow_ = o.bcancastshadow_;
  return *this;
}

void coalengine::LightComponent::SetType(LightType type)
{
  type_ = type;
}

coalengine::LightComponent::LightType coalengine::LightComponent::GetType()
{
  return type_;
}

void coalengine::LightComponent::SetPosition(glm::vec3 position)
{
  position_ = position;
}

glm::vec3 coalengine::LightComponent::GetPosition()
{
  return position_;
}

void coalengine::LightComponent::SetDirection(glm::vec3 dir)
{
  direction_ = dir;
}

glm::vec3 coalengine::LightComponent::GetDirection()
{
  return direction_;
}

void coalengine::LightComponent::SetColor(glm::vec4 color)
{
  color_.x = color.x;
  color_.y = color.y;
  color_.z = color.z;
  color_.w = color.w;
}

glm::vec4 coalengine::LightComponent::GetColor()
{
  return color_;
}

void coalengine::LightComponent::SetIntensity(float intensity)
{
  intensity_ = intensity;
}

float coalengine::LightComponent::GetIntensity()
{
  return intensity_;
}

void coalengine::LightComponent::SetOuterAngle(float angle)
{
  outer_angle_ = angle;
}

float coalengine::LightComponent::GetOuterAngle()
{
  return outer_angle_;
}

void coalengine::LightComponent::SetInnerAngle(float angle)
{
  inner_angle_ = angle;
}

float coalengine::LightComponent::GetInnerAngle()
{
  return  inner_angle_;
}

void coalengine::LightComponent::SetCanCastShadows(bool cast)
{
  if (type_ != kPoint)
  {
    bcancastshadow_ = cast;
  }
  else
  {
    bcancastshadow_ = false;
  }
}

bool coalengine::LightComponent::CanCastShadows()
{
  return bcancastshadow_;
}


