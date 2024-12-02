#ifndef  __DIPLAYLIST_H__
#define  __DIPLAYLIST_H__ 1

#include <px_mem.h>
#include <vector>
#include "Command.h"
#include "framebuffer.h"


namespace coalengine
{
  class DisplayList
  {
  public:

    DisplayList();

    DisplayList(const DisplayList& other) = delete;

    DisplayList(DisplayList&& other);

    ~DisplayList();

    DisplayList& operator=(const DisplayList& other) = delete;

    DisplayList& operator=(DisplayList&& other);

    void addCommand(px::Mem<Command>&& command);

    void clearScreen();

    void DrawAllEntities();

    void ClearEntities();

    void DrawSkyBox();

    void DpImgui();

    void ApplyPostProcess();

    void RunAll();

    void SetFramebuffer(const FrameBuffer* fbuffer);



    std::vector<px::Mem<Command>> commands_; /**< Vector that stores all the commands*/
    FrameBuffer *framebuffer_;
  };
}

#endif
