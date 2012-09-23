#include <stdexcept>
#include "common/logging.h"
#include "sdlttf.h"

using namespace std;
using namespace sdlobj;

void SDLTTF::Init() {
  if (TTF_Init()) {
    throw runtime_error(TTF_GetError());
  }
  LogDebug("SDL_ttf initialized");
}

void SDLTTF::Free() {
  TTF_Quit();
  LogDebug("SDL_ttf freed");
}

SDLTTF::~SDLTTF() {
  Free();
}
