#include <cstdlib>
#include "eventhandler.h"

using namespace sdlobj;

sdlobj::EventHandler::EventHandler() = default;

EventHandler::~EventHandler() = default;

void EventHandler::ProcessActive(const SDL_ActiveEvent &) {}

void EventHandler::ProcessKeyDown(const SDL_KeyboardEvent &) {}

void EventHandler::ProcessKeyUp(const SDL_KeyboardEvent &) {}

void EventHandler::ProcessMouseMotion(const SDL_MouseMotionEvent &) {}

void EventHandler::ProcessMouseButtonDown(const SDL_MouseButtonEvent &) {}

void EventHandler::ProcessMouseButtonUp(const SDL_MouseButtonEvent &) {}

void EventHandler::ProcessJoyAxis(const SDL_JoyAxisEvent &) {}

void EventHandler::ProcessJoyBall(const SDL_JoyBallEvent &) {}

void EventHandler::ProcessJoyHat(const SDL_JoyHatEvent &) {}

void EventHandler::ProcessJoyButtonDown(const SDL_JoyButtonEvent &) {}

void EventHandler::ProcessJoyButtonUp(const SDL_JoyButtonEvent &) {}

void EventHandler::ProcessResize(const SDL_ResizeEvent &) {}

void EventHandler::ProcessExpose(const SDL_ExposeEvent &) {}

void EventHandler::ProcessQuit(const SDL_QuitEvent &) {
  exit(EXIT_SUCCESS);
}

void EventHandler::ProcessUser(const SDL_UserEvent &) {}

void EventHandler::ProcessSysWM(const SDL_SysWMEvent &) {}
