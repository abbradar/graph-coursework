#ifndef SDLOBJ_FONT_H_
#define SDLOBJ_FONT_H_

#include <memory>
#include <SDL_ttf.h>
#include "surface.h"

namespace sdlobj {

class Font {
 public:
  Font() : font_(new FontWrapper()) { }
  Font(const char *file, int ptsize, long index = 0);

  Font(TTF_Font *font) : font_(new FontWrapper(font)) {}
  Font(const Font &other) : font_(other.font_) {}

  inline const TTF_Font *font() {
    return font_->font;
  }

  Surface RenderUTF8_Solid(const char *text, SDL_Color fg);

  int height();
  int line_skip();

 private:
  struct FontWrapper {
    FontWrapper() {}
    FontWrapper(TTF_Font *font_) : font(font_) {}
    ~FontWrapper();

    TTF_Font *font = nullptr;
  };

  std::shared_ptr<FontWrapper> font_;
};

}

#endif // SDLOBJ_FONT_H_
