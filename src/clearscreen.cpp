#include <clearscreen.h>
#include <glad/glad.h>

coalengine::ClearScreen::ClearScreen()
{
  color_[1] = 0.9f;
  color_[2] = 0.9f;
  color_[3] = 0.9f;
  color_[0] = 0.9f;
}

coalengine::ClearScreen::~ClearScreen()
{
}

void coalengine::ClearScreen::run()
{
  glClearColor(color_[0], color_[1], color_[2], color_[3]);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void coalengine::ClearScreen::SetColor(unsigned char* color)
{
  color_[0] = (float)(color[0]/255);
  color_[1] = (float)(color[1]/255);
  color_[2] = (float)(color[2]/255);
  color_[3] = (float)(color[3]/255);
}

void coalengine::ClearScreen::SetColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
  color_[0] = (float)(r/255);
  color_[1] = (float)(g/255);
  color_[2] = (float)(b/255);
  color_[3] = (float)(a/255);
}
