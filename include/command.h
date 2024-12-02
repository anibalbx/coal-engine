#ifndef __COMMANDLIST_H__
#define __COMMANDLIST_H__ 1

namespace coalengine
{
  // Virtual command class 
  class Command
  {
  public:

    virtual ~Command() {}

    // Will be overrided to provide multiple functionalities to different commands.
    virtual void run() = 0;
  };
}

#endif