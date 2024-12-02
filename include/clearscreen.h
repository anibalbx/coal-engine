#ifndef __CLEARSCREEN_H__
#define __CLEARSCREEN_H__ 1

#include "Command.h"

namespace coalengine
{
  class ClearScreen : public Command
  {
  public:
    ClearScreen();
    ClearScreen(const ClearScreen& other) = delete;
    ClearScreen(ClearScreen&& other) = default;
    ClearScreen& operator=(const ClearScreen& other) = delete;
    ClearScreen& operator=(ClearScreen&& other) = default;
    ~ClearScreen();

    // Command that clears the screen.
    void run() override;
    
    void SetColor(unsigned char* color);
   
    void SetColor(unsigned char r, unsigned char g,
                  unsigned char b, unsigned char a);

    //clear screen color.
    float color_[4];
  };
}

#endif