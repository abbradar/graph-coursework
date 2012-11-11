#ifndef SDLOBJ_FONT_H_
#define SDLOBJ_FONT_H_

#include <istream>
#include <memory>
#include <SDL_ttf.h>
#include "surface.h"
#include "color.h"

namespace sdlobj {

class Font {
public:
  Font();

  Font(std::unique_ptr<std::istream> &&in, const unsigned int ptsize, const long index = 0);

  Font(TTF_Font *font);
  Font(const Font &other);

  ~Font();

  inline const TTF_Font *const font() {
    return font_struct_;
  }

  Surface RenderUTF8_Solid(const char *text, sdlobj::Color fg);
  Surface RenderUTF8_Shaded(const char *text, sdlobj::Color fg, sdlobj::Color bg);
  Surface RenderUTF8_Blended(const char *text, sdlobj::Color fg);

  unsigned int height();
  unsigned int line_skip();

 private:
  struct FontWrapper {
    FontWrapper();
    FontWrapper(TTF_Font *font_);
    ~FontWrapper();

    TTF_Font *font;
  };

  std::shared_ptr<FontWrapper> font_;
  TTF_Font *font_struct_;
};

}

#endif // SDLOBJ_FONT_H_
