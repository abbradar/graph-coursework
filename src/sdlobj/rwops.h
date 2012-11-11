#ifndef SDLOBJ_RWOPS_H_
#define SDLOBJ_RWOPS_H_

#include <iostream>
#include <memory>
#include <SDL/SDL_rwops.h>

namespace sdlobj {

SDL_RWops *RWopsFromInputStream(std::unique_ptr<std::istream> &&in);
SDL_RWops *RWopsFromOutputStream(std::unique_ptr<std::ostream> &&out);

}

#endif // SDLOBJ_RWOPS_H_
