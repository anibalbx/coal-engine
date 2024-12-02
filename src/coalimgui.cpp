#include "coalimgui.h"
#include "imgui/ImGuizmo.h"

coalengine::CoalImgui::CoalImgui()
{
}

coalengine::CoalImgui::~CoalImgui()
{
}

void coalengine::CoalImgui::InitImgui(Window* win)
{
  const char* glsl_version = "#version 130";
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
  {
    printf("[LOG] Load GLAD error!\n");
  }

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;

  // Enable Docking
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

  // Load info from .ini.
  io.IniFilename = "../../../data/coalimgui.ini";

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer bindings
  ImGui_ImplGlfw_InitForOpenGL(win->window_, true);
  ImGui_ImplOpenGL3_Init(glsl_version);
}

void coalengine::CoalImgui::ImguiNewFrame()
{
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  ImGuizmo::BeginFrame();
}

void coalengine::CoalImgui::ImguiDraw()
{
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void coalengine::CoalImgui::ImguiDestroy()
{
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}
