#ifndef SDLOBJ_EVENTHANDLER_H_
#define SDLOBJ_EVENTHANDLER_H_

#include <SDL.h>

namespace sdlobj {

class EventHandler {
 public:
  EventHandler() = default;
  virtual ~EventHandler() {}
  EventHandler(const EventHandler &) = delete;

  virtual void ProcessActive(const SDL_ActiveEvent &) {}
  virtual void ProcessKeyDown(const SDL_KeyboardEvent &) {}
  virtual void ProcessKeyUp(const SDL_KeyboardEvent &) {}
  virtual void ProcessMouseMotion(const SDL_MouseMotionEvent &) {}
  virtual void ProcessMouseButtonDown(const SDL_MouseButtonEvent &) {}
  virtual void ProcessMouseButtonUp(const SDL_MouseButtonEvent &) {}
  virtual void ProcessJoyAxis(const SDL_JoyAxisEvent &) {}
  virtual void ProcessJoyBall(const SDL_JoyBallEvent &) {}
  virtual void ProcessJoyHat(const SDL_JoyHatEvent &) {}
  virtual void ProcessJoyButtonDown(const SDL_JoyButtonEvent &) {}
  virtual void ProcessJoyButtonUp(const SDL_JoyButtonEvent &) {}
  virtual void ProcessResize(const SDL_ResizeEvent &) {}
  virtual void ProcessExpose(const SDL_ExposeEvent &) {}
  virtual void ProcessQuit(const SDL_QuitEvent &) { exit(0); }
  virtual void ProcessUser(const SDL_UserEvent &) {}
  virtual void ProcessSysWM(const SDL_SysWMEvent &) {}
};

}

#endif // SDLOBJ_EVENTHANDLER_H_
