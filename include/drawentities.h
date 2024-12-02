#ifndef __DRAW_ENTITIES_H__
#define __DRAW_ENTITIES_H__ 1

#include "Command.h"

namespace coalengine
{
  class DrawEntities : public Command
  {
  public:
    DrawEntities();
    ~DrawEntities();
  
    void run() override;
  };
}

#endif


