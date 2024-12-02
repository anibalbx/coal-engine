#ifndef __CLEARDRAWENTITIES_H__
#define __CLEARDRAWENTITIES_H__ 1

#include "Command.h"

namespace coalengine
{
 
  class ClearDrawEntities : public Command
  {
  public:
    ClearDrawEntities();
    ~ClearDrawEntities();

    // Clear the entities in the entities to draw vector at the end of frame.
    void run() override;
  };
}

#endif


