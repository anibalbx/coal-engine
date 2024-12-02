#ifndef __DRAWIMGUI_H__
#define __DRAWIMGUI_H__ 1


#include <string>
#include <vector>
#include "imgui/imgui.h"
#include "Command.h"
#include <imgui/ImGuizmo.h>

namespace coalengine
{
  class DrawImgui : public Command
  {
  public:
    DrawImgui();
    ~DrawImgui();
   
    void run() override;

    // Imgui list box wraper.
    bool ListBox(const char* label, int* currIndex, std::vector<std::string>& values);

    void PrintTRS(float position[3],float rotation[3], float scale[3], bool editable);

    ImGuiID dockspace_id_; // id for the docking windows.



  };
}

#endif


