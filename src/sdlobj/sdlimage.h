#ifndef SDLOBJ_SDLIMAGE_H_
#define SDLOBJ_SDLIMAGE_H_

#include <memory>
#include <istream>
#include <SDL_image.h>
#include "common/singleton.h"
#include "sdlobj/surface.h"

namespace sdlobj {

class SDLImage : public Singleton<SDLImage> {
 public:
  void Init(const Uint32 flags);
  void Free();

  static Surface Load(std::unique_ptr<std::istream> &&in);

  inline bool initialized() {
    return initialized_;
  }

  ~SDLImage();

 private:
  friend class Singleton<SDLImage>;

  bool initialized_;

  SDLImage();
};

}

#endif // SDLOBJ_SDLIMAGE_H_
