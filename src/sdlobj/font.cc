#include "common/exception.h"
#include "sdlttf.h"
#include "font.h"

using namespace std;
using namespace sdlobj;

Font::Font() : font_() { }
Font::Font(TTF_Font *font) : font_(new FontWrapper(font)) {}
Font::Font(const Font &other) : font_(other.font_) {}

Font::Font(const char *file, int ptsize, long index) : font_(new FontWrapper()) {
  font_->font = TTF_OpenFontIndex(file, ptsize, index);
  if (!font_->font) {
    throw Exception(TTF_GetError());
  }
}

Font::~Font() {
  if (!SDLTTF::instance().initialized() && font_) {
    font_->font = nullptr;
  }
}

Surface Font::RenderUTF8_Solid(const char *text, Color fg) {
  return Surface(TTF_RenderUTF8_Solid(font_->font, text, fg));
}

Surface Font::RenderUTF8_Shaded(const char *text, Color fg, Color bg) {
  return Surface(TTF_RenderUTF8_Shaded(font_->font, text, fg, bg));
}

Surface Font::RenderUTF8_Blended(const char *text, Color fg) {
  return Surface(TTF_RenderUTF8_Blended(font_->font, text, fg));
}

int Font::height() {
  return TTF_FontHeight(font_->font);
}

int Font::line_skip() {
  return TTF_FontLineSkip(font_->font);
}

Font::FontWrapper::~FontWrapper() {
  if (font) TTF_CloseFont(font);
}
