#include "color.h"

namespace sdlobj {

Color::Color() : Color(0, 0, 0, 0) {}

Color::Color(Uint8 r_, Uint8 g_, Uint8 b_, Uint8 a_) : SDL_Color({.r = r_, .g = g_, .b = b_, .unused = a_}) {}

}
