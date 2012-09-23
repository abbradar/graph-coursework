#include <stdexcept>
#include <SDL.h>
#include <boost/format.hpp>
#include "common/logging.h"
#include "sdl.h"

using namespace std;
using namespace sdlobj;

void SDL::Init(Uint32 flags) {
  if (SDL_Init(flags)) {
    throw runtime_error(SDL_GetError());
  }
  initialized_ = true;
  LogDebug("SDL initialized");
}

void DummyDelete(SDL_Surface *) {}

void SDL::Free() {
  initialized_ = false;
  surface_.surface_->surface = nullptr;
  SDL_Quit();
  LogDebug("SDL freed");
}

void SDL::SetVideoMode(int width, int height, int bpp, Uint32 flags) {
  surface_.surface_->surface = SDL_SetVideoMode(width, height, bpp, flags);
  LogDebug((boost::format("Video mode setted: %1%x%2%@%3%") % width % height % bpp).str().data());

  if (!surface_.surface_) {
    throw runtime_error(SDL_GetError());
  }
}


SDL::~SDL() {
  Free();
}

void SDL::WaitEvent()
{
  SDL_Event event;

  SDL_WaitEvent(&event);
  ProcessEvent(event);
}

bool SDL::PollEvent()
{
  SDL_Event event;

  if (SDL_PollEvent(&event)) {
    ProcessEvent(event);
    return true;
  } else {
    return false;
  }
}

void SDL::Flip() {
  if (SDL_Flip(surface_.surface_->surface)) {
    throw runtime_error(SDL_GetError());
  }
}

void SDL::UpdateRect(int x, int y, int width, int height) {
  SDL_UpdateRect(surface_.surface_->surface, x, y, width, height);
}

void SDL::ProcessEvent(const SDL_Event &event)
{
  switch (event.type) {
    case SDL_ACTIVEEVENT:
      event_handler_->ProcessActive(event.active);
      break;
    case SDL_KEYDOWN:
      event_handler_->ProcessKeyDown(event.key);
      break;
    case SDL_KEYUP:
      event_handler_->ProcessKeyUp(event.key);
      break;
    case SDL_MOUSEMOTION:
      event_handler_->ProcessMouseMotion(event.motion);
      break;
    case SDL_MOUSEBUTTONDOWN:
      event_handler_->ProcessMouseButtonDown(event.button);
      break;
    case SDL_MOUSEBUTTONUP:
      event_handler_->ProcessMouseButtonUp(event.button);
      break;
    case SDL_JOYAXISMOTION:
      event_handler_->ProcessJoyAxis(event.jaxis);
      break;
    case SDL_JOYBALLMOTION:
      event_handler_->ProcessJoyBall(event.jball);
      break;
    case SDL_JOYHATMOTION:
      event_handler_->ProcessJoyHat(event.jhat);
      break;
    case SDL_JOYBUTTONDOWN:
      event_handler_->ProcessJoyButtonDown(event.jbutton);
      break;
    case SDL_JOYBUTTONUP:
      event_handler_->ProcessJoyButtonUp(event.jbutton);
      break;
    case SDL_VIDEORESIZE:
      event_handler_->ProcessResize(event.resize);
      break;
    case SDL_VIDEOEXPOSE:
      event_handler_->ProcessExpose(event.expose);
      break;
    case SDL_QUIT:
      event_handler_->ProcessQuit(event.quit);
      break;
    case SDL_USEREVENT:
      event_handler_->ProcessUser(event.user);
      break;
    case SDL_SYSWMEVENT:
      event_handler_->ProcessSysWM(event.syswm);
      break;
  }
}
