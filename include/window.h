#ifndef __WINDOW_H__
#define __WINDOW_H__ 1
#include "glfw3.h"
#include <types.h>


class Window
{
public:

  GLFWwindow* window_;


  Window();

  ~Window();


  bool init(uint16 width, uint16 height);

  bool init();

  bool windowIsOpen();

  bool processEvents();

  void swap();

  void finish();

  int getWidth();

  int getHeight();

  void setWidth(uint16 width);

  void setHeight(uint16 height);

private:

  uint16 width_;
  uint16 height_;
};

#endif
