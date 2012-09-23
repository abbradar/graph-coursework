#include <stdexcept>
#include "sdlobj/sdlttf.h"
#include "font.h"

using namespace std;
using namespace sdlobj;

Font::Font(const char *file, int ptsize, long index) : Font() {
  font_->font = TTF_OpenFontIndex(file, ptsize, index);
  if (!font_->font) {
    throw runtime_error(TTF_GetError());
  }
}

Surface Font::RenderUTF8_Solid(const char *text, SDL_Color fg) {
  return Surface(TTF_RenderUTF8_Solid(font_->font, text, fg));
}

Surface Font::RenderUTF8_Blended(const char *text, SDL_Color fg) {
  return Surface(TTF_RenderUTF8_Blended(font_->font, text, fg));
}

int Font::height() {
  return TTF_FontHeight(font_->font);
}

int Font::line_skip() {
  return TTF_FontLineSkip(font_->font);
}

Font::FontWrapper::~FontWrapper() {
  TTF_CloseFont(font);
}
