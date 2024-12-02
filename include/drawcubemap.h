#ifndef __DRAW_CUBEAMP_H__
#define __DRAW_CUBEAMP_H__ 1

#include <string>
#include <glad/glad.h>
#include "Command.h"

namespace coalengine
{
  class DrawCubeMap : public Command
  {
  public:
    DrawCubeMap();
    ~DrawCubeMap();
 
    void run() override;
  };
}

#endif


