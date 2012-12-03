#include "common/exception.h"
#include "sdlttf.h"
#include "rwops.h"
#include "font.h"

using namespace std;

namespace sdlobj {

Font::Font() : font_(), font_struct_(nullptr) {}
Font::Font(TTF_Font *font) : font_(new FontWrapper(font)), font_struct_(font_->font) {}
Font::Font(const Font &other) : font_(other.font_), font_struct_(other.font_struct_ ) {}

Font &Font::operator =(const Font &other) {
  font_ = other.font_;
  font_struct_ = other.font_struct_;
  return *this;
}

Font::Font(std::unique_ptr<istream> &&in, const unsigned int ptsize, const long index) : font_(new FontWrapper()) {
  SDL_RWops *rwops = RWopsFromInputStream(forward<std::unique_ptr<istream>>(in));
  font_->font = TTF_OpenFontIndexRW(rwops, 1, ptsize, index);
  if (!font_->font) {
    throw Exception(TTF_GetError());
  }
  font_struct_ = font_->font;
}

Font::~Font() {
  if (!SDLTTF::instance().initialized() && font_) {
    font_->font = nullptr;
  }
}

Surface Font::RenderUTF8_Solid(const char *text, Color fg) const {
  if (text[0] != '\0') {
    SDL_Surface * surface = TTF_RenderUTF8_Solid(font_struct_, text, fg);
    if (!surface) {
      throw Exception(TTF_GetError());
    }
    return Surface(surface);
  } else {
    return Surface(0, 0);
  }
}

Surface Font::RenderUTF8_Shaded(const char *text, Color fg, Color bg) const {
  if (text[0] != '\0') {
    SDL_Surface * surface = TTF_RenderUTF8_Shaded(font_struct_, text, fg, bg);
    if (!surface) {
      throw Exception(TTF_GetError());
    }
    return Surface(surface);
  } else {
    return Surface(0, 0);
  }
}

Surface Font::RenderUTF8_Blended(const char *text, Color fg) const {
  if (text[0] != '\0') {
    SDL_Surface * surface = TTF_RenderUTF8_Blended(font_struct_, text, fg);
    if (!surface) {
      throw Exception(TTF_GetError());
    }
    return Surface(surface);
  } else {
    return Surface(0, 0);
  }
}

void Font::SizeUTF8(const char *text, unsigned *w, unsigned *h) const {
  if (text[0] != '\0') {
    if (TTF_SizeUTF8(font_struct_, text, (int *)w, (int *)h) != 0) {
      throw Exception(TTF_GetError());
    }
  } else {
    if (w) *w = 0;
    if (h) *h = 0;
  }
}

unsigned int Font::height() const {
  return TTF_FontHeight(font_struct_);
}

unsigned int Font::line_skip() const {
  return TTF_FontLineSkip(font_struct_);
}

Font::FontWrapper::FontWrapper() : font(nullptr) {}
Font::FontWrapper::FontWrapper(TTF_Font *font_) : font(font_) {}

Font::FontWrapper::~FontWrapper() {
  if (font) TTF_CloseFont(font);
}

}
