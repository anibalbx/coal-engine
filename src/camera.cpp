#include "camera.h"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include "engineutils.h"
#include "renderlist.h"

coalengine::Camera::Camera()
{
  view_ = glm::lookAt(position_, position_ + front_, up_);
  projection_ = glm::perspective(glm::radians(45.0f), (1920.0f / 1080.0f), 0.1f, 200.0f);

  position_ = glm::vec3(0.0f, 0.0f, 3.0f);
  front_ = glm::vec3(0.0f, 0.0f, -1.0f);
  up_ = glm::vec3(0.0f, 1.0f, 0.0f);
  speed_ = 2.0f;
  first_mouse_ = true;

  yaw_ = -90.0f;
  pitch_ = 0.0f;
  lastX_ = EngineUtils::Instance().width / 2.0f;
  lastY_ = EngineUtils::Instance().height / 2.0f;
  bloom_active_ = false;
}

coalengine::Camera::~Camera()
{
}

coalengine::Camera::Camera(Camera&& o)
{
  view_ = o.view_;
  projection_ = o.projection_;
  position_ = o.position_;
  front_ = o.front_;
  up_ = o.up_;
  speed_ = o.speed_;
  yaw_ = o.yaw_;
  pitch_ = o.pitch_;
  lastX_ = o.lastX_;
  lastY_ = o.lastY_;

}

coalengine::Camera& coalengine::Camera::operator=(Camera&& o)
{
  view_ = o.view_;
  projection_ = o.projection_;
  position_ = o.position_;
  front_ = o.front_;
  up_ = o.up_;
  speed_ = o.speed_;
  yaw_ = o.yaw_;
  pitch_ = o.pitch_;
  lastX_ = o.lastX_;
  lastY_ = o.lastY_;

  return  *this;
}

void coalengine::Camera::SetPosition(glm::vec3 pos)
{
  position_ = pos;
}

glm::vec3 coalengine::Camera::GetPosition()
{
  return position_;
}

void coalengine::Camera::SetFront(glm::vec3 front)
{
  front_ = front;
}

glm::vec3 coalengine::Camera::GetFront()
{
  return front_;
}

void coalengine::Camera::SetUp(glm::vec3 up)
{
  up_ = up;
}

glm::vec3 coalengine::Camera::GetUp()
{
  return up_;
}

void coalengine::Camera::SetSpeed(float speed)
{
  speed_ = speed;
}

float coalengine::Camera::GetSpeed()
{
  return  speed_;
}

void coalengine::Camera::SetPitch(double pitch)
{
  pitch_ = pitch;
}

double coalengine::Camera::GetPitch()
{
  return pitch_;
}

void coalengine::Camera::SetYaw(double yaw)
{
  yaw_ = yaw;
}

double coalengine::Camera::GetYaw()
{
  return  yaw_;
}

void coalengine::Camera::SetLastX(double x)
{
  lastX_ = x;
}

double coalengine::Camera::GetLastX()
{
  return lastX_;
}

void coalengine::Camera::SetLastY(double y)
{
  lastY_ = y;
}

double coalengine::Camera::GetLastY()
{
  return  lastY_;
}

void coalengine::Camera::EnableBloom()
{
  bloom_active_ = true;
  RenderList::Instance().CallClearFrame();
}

void coalengine::Camera::DisableBloom()
{
  bloom_active_ = false;
  RenderList::Instance().CallClearFrame();

}

void coalengine::Camera::AddPostprocess(PostProcess::PostProcessType type)
{
  px::Mem<PostProcess> post_process;
  post_process.alloc();
  post_process->type_ = type;
  RenderList::Instance().post_process_list_.push_back(std::move(post_process));
}

void coalengine::Camera::SetSkyBox(std::vector<std::string> textures)
{
  RenderList::Instance().sky_box_.textures = textures;
}



