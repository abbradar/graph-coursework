#include "application.h"
#include "logging.h"
#include "coursewindow.h"

void CourseWindow::ProcessQuit(const SDL_QuitEvent &)
{
  LogDebug("Quit event received; exiting");
  Application::instance().Terminate(0);
}
