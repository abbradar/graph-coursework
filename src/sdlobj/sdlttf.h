#ifndef SDLOBJ_SDLTTF_H_
#define SDLOBJ_SDLTTF_H_

#include "common/singleton.h"

namespace sdlobj {

class SDLTTF : public Singleton<SDLTTF> {
 public:
  void Init();
  void Free();

  bool initialized();

  ~SDLTTF();

 private:
  friend class Singleton<SDLTTF>;

  SDLTTF();
};

}

#endif // SDLOBJ_SDL_H_
