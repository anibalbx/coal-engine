#ifndef __ENGINEUTILS_H__
#define __ENGINEUTILS_H__ 1

#include <vec2.hpp>
#include <vector>
#include "boundingbox.h"
#include "entity.h"
#include "timer.h"
#include "types.h"
#include "imgui/imgui.h"
#include "imgui/ImGuizmo.h"

static const float PI = 3.14159265f;

namespace coalengine
{

  // General engine utilities.
  class EngineUtils
  {
  public:

    static EngineUtils& Instance();

    EngineUtils();

    EngineUtils(const EngineUtils& o) = default;

    ~EngineUtils();

    void StartFrameTimer();

    void EndFrameTimer();

    bool LoadStringFromFile(const char* filepath, std::string& str);

    void AddToLog(std::string text);

    std::string ReturnLog();

    void GenerateSphere(int stacks, int sectors, float radius, std::vector<float>& vertices,
      std::vector<float>& normals, std::vector<float>& uv, std::vector<unsigned int>& index);

    void GeneratePlane(std::vector<float>& vertex, std::vector<float>& normals,
      std::vector<float>& uv, std::vector<unsigned int>& index);

    void GenerateCube(std::vector<float>& vertex, std::vector<float>& normals,
      std::vector<float>& uv, std::vector<unsigned int>& index);

    BoundingBox GenerateBoundingBox(std::vector<float> vertex);

    uint16 width; // screen width.
    uint16 height; // screen height.
    double frame_time_;
    Timer timer_;
    bool show_engine_front_; // Full screen.
    glm::vec2 world_window_size; // World editor window size.
    glm::vec2 world_window_pos; // World editor window position.
    bool active_deferred_;
    bool play_update_;

    //Front End Variables

    int entity_id_sel_; // current selected entity 
    std::vector<std::string> light_types;
    std::vector<std::string> id_vector_;
    std::vector<Entity*> entities_ptr_;

    std::string coal_log_string_;

    ImGuizmo::MODE current_guizmo_mode_;
    ImGuizmo::OPERATION current_guizmo_opeation_;
  };
}



#endif



