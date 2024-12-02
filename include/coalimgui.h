#ifndef __COALIMGUI_H__
#define __COALIMGUI_H__ 1
#include <cstdio>
#include <glad/glad.h>
#include <imgui.h>
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "window.h"

/** @author Aníbal SG
*/

namespace coalengine
{
  class CoalImgui
  {
  public:
    CoalImgui();

    ~CoalImgui();
  
    void InitImgui(Window* win);

    void ImguiNewFrame();

    void ImguiDraw();
  
    void ImguiDestroy();
  };

}

#endif
