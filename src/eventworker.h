#ifndef GRAPH_EVENTRECEIVER_H_
#define GRAPH_EVENTRECEIVER_H_

#include <sdlobj/sdl.h>
#include "conveyorworker.h"

class EventWorker : virtual public ConveyorWorker {
 public:
  EventWorker();
  virtual ~EventWorker();

  virtual bool ProcessActive(const SDL_ActiveEvent &);
  virtual bool ProcessKeyDown(const SDL_KeyboardEvent &);
  virtual bool ProcessKeyUp(const SDL_KeyboardEvent &);
  virtual bool ProcessMouseMotion(const SDL_MouseMotionEvent &);
  virtual bool ProcessMouseButtonDown(const SDL_MouseButtonEvent &);
  virtual bool ProcessMouseButtonUp(const SDL_MouseButtonEvent &);
  virtual bool ProcessJoyAxis(const SDL_JoyAxisEvent &);
  virtual bool ProcessJoyBall(const SDL_JoyBallEvent &);
  virtual bool ProcessJoyHat(const SDL_JoyHatEvent &);
  virtual bool ProcessJoyButtonDown(const SDL_JoyButtonEvent &);
  virtual bool ProcessJoyButtonUp(const SDL_JoyButtonEvent &);
  virtual bool ProcessResize(const SDL_ResizeEvent &);
  virtual bool ProcessExpose(const SDL_ExposeEvent &);
  virtual bool ProcessQuit(const SDL_QuitEvent &);
  virtual bool ProcessUser(const SDL_UserEvent &);
  virtual bool ProcessSysWM(const SDL_SysWMEvent &);

  virtual void EventStep();

  virtual EventWorker *event_worker();
};

#endif // GRAPH_EVENTRECEIVER_H_
