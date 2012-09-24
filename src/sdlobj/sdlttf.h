#ifndef SDLOBJ_SDLTTF_H_
#define SDLOBJ_SDLTTF_H_

#include <SDL_ttf.h>
#include "common/singleton.h"

namespace sdlobj {

class SDLTTF : public Singleton<SDLTTF> {
 public:
  void Init();
  void Free();

  bool initialized() {
    return TTF_WasInit() != 0;
  }

  ~SDLTTF();

 private:
  friend class Singleton<SDLTTF>;

  SDLTTF();
};

}

#endif // SDLOBJ_SDL_H_
