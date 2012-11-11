#include <SDL_ttf.h>
#include "common/logging.h"
#include "common/exception.h"
#include "sdlttf.h"

using namespace std;

namespace sdlobj {

void SDLTTF::Init() {
  if (initialized()) return;
  if (TTF_Init()) {
    throw Exception(TTF_GetError());
  }
  LogDebug("SDL_ttf initialized");
}

void SDLTTF::Free() {
  if (!initialized()) return;
  TTF_Quit();
}

bool SDLTTF::initialized() {
  return TTF_WasInit() != 0;
}

SDLTTF::~SDLTTF() {
  Free();
}

SDLTTF::SDLTTF() = default;

}
