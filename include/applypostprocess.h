
#ifndef __APPLy_POST_PROCESS__
#define __APPLy_POST_PROCESS__ 1


#include <string>
#include <glad/glad.h>
#include "command.h"

namespace coalengine
{
  class ApplyPostprocess : public Command
  {
  public:
    ApplyPostprocess();
    ~ApplyPostprocess();

    void run() override;
  };
}

#endif


