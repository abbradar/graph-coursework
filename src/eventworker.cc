#include "eventworker.h"

EventWorker::EventWorker() = default;
EventWorker::~EventWorker() = default;

bool EventWorker::ProcessActive(const SDL_ActiveEvent &) {
  return false;
}

bool EventWorker::ProcessKeyDown(const SDL_KeyboardEvent &) {
  return false;
}

bool EventWorker::ProcessKeyUp(const SDL_KeyboardEvent &) {
  return false;
}

bool EventWorker::ProcessMouseMotion(const SDL_MouseMotionEvent &) {
  return false;
}

bool EventWorker::ProcessMouseButtonDown(const SDL_MouseButtonEvent &) {
  return false;
}

bool EventWorker::ProcessMouseButtonUp(const SDL_MouseButtonEvent &) {
  return false;
}

bool EventWorker::ProcessJoyAxis(const SDL_JoyAxisEvent &) {
  return false;
}

bool EventWorker::ProcessJoyBall(const SDL_JoyBallEvent &) {
  return false;
}

bool EventWorker::ProcessJoyHat(const SDL_JoyHatEvent &) {
  return false;
}

bool EventWorker::ProcessJoyButtonDown(const SDL_JoyButtonEvent &) {
  return false;
}

bool EventWorker::ProcessJoyButtonUp(const SDL_JoyButtonEvent &) {
  return false;
}

bool EventWorker::ProcessResize(const SDL_ResizeEvent &) {
  return false;
}

bool EventWorker::ProcessExpose(const SDL_ExposeEvent &) {
  return false;
}

bool EventWorker::ProcessQuit(const SDL_QuitEvent &) {
  return false;
}

bool EventWorker::ProcessUser(const SDL_UserEvent &) {
  return false;
}

bool EventWorker::ProcessSysWM(const SDL_SysWMEvent &) {
  return false;
}

void EventWorker::EventStep() {}

EventWorker *EventWorker::event_worker() {
  return this;
}
