#ifndef __CAMERA_H__
#define __CAMERA_H__ 1

#include <fwd.hpp>
#include <string>
#include <vector>
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include "postprocess.h"

namespace coalengine
{
  class Camera
  {
  public:
    Camera();
    virtual ~Camera();

    Camera(const Camera& o) = delete;
    Camera& operator=(const Camera& o) = delete;

    Camera(Camera&& o);
    Camera& operator=(Camera&& o);

    void SetPosition( glm::vec3 pos);
    glm::vec3 GetPosition();

    void SetFront(glm::vec3 front);
    glm::vec3 GetFront();

    void SetUp(glm::vec3 up);
    glm::vec3 GetUp();

    void SetSpeed(float speed);
    float GetSpeed();

    void SetPitch(double pitch);
    double GetPitch();

    void SetYaw(double yaw);
    double GetYaw();

    void SetLastX(double x);
    double GetLastX();

    void SetLastY(double y);
    double GetLastY();

    void EnableBloom();
    void DisableBloom();

    void AddPostprocess(PostProcess::PostProcessType type);

    void SetSkyBox(std::vector<std::string> textures);

    glm::mat4 view_;
    glm::mat4 projection_;

    // check the first time mouse is clicked in order to center the camera.
    bool first_mouse_;

    glm::vec3 position_;
    glm::vec3 front_;
    glm::vec3 up_;
    float speed_;
    double pitch_;
    double yaw_;

    // Stores the mouses previous x and y position. 
    double lastX_;
    double lastY_;

    bool bloom_active_;
  };
}


#endif

