#ifndef GRAPH_COURSEEVENTHANDLER_H_
#define GRAPH_COURSEEVENTHANDLER_H_

#include "sdlobj/eventhandler.h"

class CourseEventHandler : public sdlobj::EventHandler {
 public:
  void ProcessQuit(const SDL_QuitEvent &);
};

#endif // GRAPH_COURSEEVENTHANDLER_H_
