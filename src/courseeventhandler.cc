#include "common/logging.h"
#include "common/application.h"
#include "courseeventhandler.h"

void CourseEventHandler::ProcessQuit(const SDL_QuitEvent &) {
  LogDebug("Quit event received; exiting");
  Application::instance().Terminate(0);
}
