#include <stdexcept>
#include <boost/format.hpp>
#include <SDL_byteorder.h>
#include "logging.h"
#include "debug.h"
#include "drawingwindow.h"

using namespace std;

void set_pixel_1(Uint8 *p, Uint32 pixel) {
  *p = pixel;
}

void set_pixel_2(Uint8 *p, Uint32 pixel) {
  *(Uint16 *)p = pixel;
}

void set_pixel_3(Uint8 *p, Uint32 pixel) {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    p[0] = (pixel >> 16) & 0xFF;
    p[1] = (pixel >> 8) & 0xFF;
    p[2] = pixel & 0xFF;
#else
    p[0] = pixel & 0xFF;
    p[1] = (pixel >> 8) & 0xFF;
    p[2] = (pixel >> 16) & 0xFF;
#endif
}

void set_pixel_4(Uint8 *p, Uint32 pixel) {
  *(Uint32 *)p = pixel;
}

Uint32 get_pixel_1(Uint8 *p) {
  return *p;
}

Uint32 get_pixel_2(Uint8 *p) {
  return *(Uint16 *)p;
}

Uint32 get_pixel_3(Uint8 *p) {
  if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
    return p[0] << 16 | p[1] << 8 | p[2];
  else
    return p[0] | p[1] << 8 | p[2] << 16;
}

Uint32 get_pixel_4(Uint8 *p) {
  return *(Uint32 *)p;
}

void DrawingWindow::SetVideoMode(unsigned int x, unsigned int y, unsigned int bpp) {
  surface_ = SDL_SetVideoMode(x, y, bpp, video_mode_flags() | SDL_SWSURFACE | SDL_DOUBLEBUF);
  if (surface_ == NULL) {
    throw runtime_error((boost::format("Unable to set video mode: %1%") % SDL_GetError()).str().data());
  }

  int bytes = surface_->format->BytesPerPixel;

  switch (bytes) {
   case 1:
    get_pixel_ = &get_pixel_1;
    set_pixel_ = &set_pixel_1;
    break;
   case 2:
    get_pixel_ = &get_pixel_2;
    set_pixel_ = &set_pixel_2;
    break;
   case 3:
    get_pixel_ = &get_pixel_3;
    set_pixel_ = &set_pixel_3;
    break;
   case 4:
    get_pixel_ = &get_pixel_4;
    set_pixel_ = &set_pixel_4;
    break;
   default:
    AssertMsg(false, "Unknown bpp value");
  }

  LogDebug((boost::format("Video mode setted: %1%x%2%:%3%") % x % y % bpp).str().data());
}

void DrawingWindow::StartDrawing()
{
  SDL_LockSurface(surface_);
}

void DrawingWindow::EndDrawing()
{
  SDL_UnlockSurface(surface_);
  SDL_Flip(surface_);
}

void DrawingWindow::WaitEvent()
{
  SDL_Event event;

  SDL_WaitEvent(&event);
  ProcessEvent(event);
}

bool DrawingWindow::PollEvent()
{
  SDL_Event event;

  if (SDL_PollEvent(&event)) {
    ProcessEvent(event);
    return true;
  } else {
    return false;
  }
}

void DrawingWindow::ProcessEvent(const SDL_Event &event)
{
  switch (event.type) {
    case SDL_ACTIVEEVENT:
      ProcessActive(event.active);
      break;
    case SDL_KEYDOWN:
      ProcessKeyDown(event.key);
      break;
    case SDL_KEYUP:
      ProcessKeyUp(event.key);
      break;
    case SDL_MOUSEMOTION:
      ProcessMouseMotion(event.motion);
      break;
    case SDL_MOUSEBUTTONDOWN:
      ProcessMouseButtonDown(event.button);
      break;
    case SDL_MOUSEBUTTONUP:
      ProcessMouseButtonUp(event.button);
      break;
    case SDL_JOYAXISMOTION:
      ProcessJoyAxis(event.jaxis);
      break;
    case SDL_JOYBALLMOTION:
      ProcessJoyBall(event.jball);
      break;
    case SDL_JOYHATMOTION:
      ProcessJoyHat(event.jhat);
      break;
    case SDL_JOYBUTTONDOWN:
      ProcessJoyButtonDown(event.jbutton);
      break;
    case SDL_JOYBUTTONUP:
      ProcessJoyButtonUp(event.jbutton);
      break;
    case SDL_VIDEORESIZE:
      ProcessResize(event.resize);
      break;
    case SDL_VIDEOEXPOSE:
      ProcessExpose(event.expose);
      break;
    case SDL_QUIT:
      ProcessQuit(event.quit);
      break;
    case SDL_USEREVENT:
      ProcessUser(event.user);
      break;
    case SDL_SYSWMEVENT:
      ProcessSysWM(event.syswm);
      break;
  }
}
