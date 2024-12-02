#ifndef __LIGHT_COMPONENT_H__
#define __LIGHT_COMPONENT_H__ 1

#include <fwd.hpp>
#include <vec3.hpp>
#include <mat4x4.hpp>


namespace coalengine
{
  class LightComponent
  {
  public:

    enum LightType
    {
      kNone,
      kDirectional,
      kSpot,
      kPoint,
    };

    LightComponent();
    LightComponent(const LightComponent& o) = delete;
    LightComponent(LightComponent&& o);
    ~LightComponent();

    LightComponent& operator= (const LightComponent& o) = delete;
    LightComponent& operator= (LightComponent&& o);


    void SetType(LightType type);
    LightType GetType();

    void SetPosition(glm::vec3 position);
    glm::vec3 GetPosition();

    void SetDirection(glm::vec3 dir);
    glm::vec3 GetDirection();

    void SetColor(glm::vec4 color);
    glm::vec4 GetColor();

    void SetIntensity(float intensity);
    float GetIntensity();

    void SetOuterAngle(float angle);
    float GetOuterAngle();

    void SetInnerAngle(float angle);
    float GetInnerAngle();

    void SetCanCastShadows(bool cast);
    bool CanCastShadows();

    glm::mat4 light_view_;
    glm::mat4 light_space_; // space matrix in order to make shadows.
    glm::mat4 model_;
    glm::mat4 projection_;

  private:
    LightType type_;
    glm::vec3 position_;
    glm::vec3 direction_;
    glm::vec4 color_;
    float intensity_;
    float outer_angle_;
    float inner_angle_;
    bool bcancastshadow_;
  };
}

#endif 

