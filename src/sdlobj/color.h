#ifndef SDLOBJ_COLOR_H_
#define SDLOBJ_COLOR_H_

#include <SDL_video.h>

namespace sdlobj {

struct Color : public SDL_Color {
 public:
  Color();
  Color(Uint8 r, Uint8 g, Uint8 b);
};

}

#endif // SDLOBJ_COLOR_H_
