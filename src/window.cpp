#include "window.h"

Window::Window()
{
  width_ = 0;
  height_ = 0;
}

Window::~Window()
{
}

bool Window::init()
{
  /* Initialize the library */
  if (!glfwInit())
    return false;

  window_ = glfwCreateWindow(getWidth(), getHeight(), "CoalEngine", NULL, NULL);
  if (!window_)
  {
    glfwTerminate();
    return false;
  }

  glfwMakeContextCurrent(window_);
}

bool Window::windowIsOpen()
{
  return glfwWindowShouldClose(window_);
}


bool Window::init(uint16 width, uint16 height)
{
  if (!glfwInit())
    return false;

  window_ = glfwCreateWindow(width, height, "CoalEngine", NULL, NULL);
  if (!window_)
  {
    glfwTerminate();
    return false;
  }

  glfwMakeContextCurrent(window_);
  //Disablevsync
  //glfwSwapInterval(0);
}


bool Window::processEvents()
{
  glfwPollEvents();
  return true;
}

void Window::swap()
{
  glfwSwapBuffers(window_);
}

void Window::finish()
{
  glfwTerminate();
}

int Window::getWidth()
{
  return width_;
}
int Window::getHeight()
{
  return height_;
}

void Window::setWidth(uint16 width)
{
  this->width_ = width;
}

void Window::setHeight(uint16 height)
{
  this->height_ = height;
}
