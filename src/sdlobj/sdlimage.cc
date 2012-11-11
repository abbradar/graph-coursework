#include "common/logging.h"
#include "common/exception.h"
#include "rwops.h"
#include "sdlimage.h"

using namespace std;

namespace sdlobj {

void SDLImage::Init(const Uint32 flags) {
  if (initialized()) return;
  if ((IMG_Init(flags) & flags) != flags) {
    throw Exception(IMG_GetError());
  }
  initialized_ = true;
  LogDebug("SDL_image initialized");
}

void SDLImage::Free() {
  if (initialized()) return;
  IMG_Quit();
  initialized_ = false;
}

Surface SDLImage::Load(std::unique_ptr<istream> &&in) {
  SDL_RWops *rwops = RWopsFromInputStream(forward<std::unique_ptr<istream>>(in));
  SDL_Surface *surface = IMG_Load_RW(rwops, 1);
  if (!surface) {
    throw Exception(IMG_GetError());
  }
  return Surface(surface);
}

SDLImage::~SDLImage() {
  Free();
}

SDLImage::SDLImage() : initialized_(false) {}

}
