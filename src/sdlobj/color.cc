#include "color.h"

using namespace sdlobj;

Color::Color() : SDL_Color({.r = 0, .g = 0, .b = 0}) {}

Color::Color(Uint8 r_, Uint8 g_, Uint8 b_) : SDL_Color({.r = r_, .g = g_, .b = b_}) {}
