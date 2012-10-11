#include "common/exception.h"
#include "sdlttf.h"
#include "font.h"

using namespace std;
using namespace sdlobj;

Font::Font() : font_(), font_struct_(nullptr) {}
Font::Font(TTF_Font *font) : font_(new FontWrapper(font)), font_struct_(font_->font) {}
Font::Font(const Font &other) : font_(other.font_), font_struct_(nullptr) {
  if (font_) {
    font_struct_ = font_->font;
  }
}

Font::Font(const char *file, int ptsize, long index) : font_(new FontWrapper()) {
  font_->font = TTF_OpenFontIndex(file, ptsize, index);
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

Surface Font::RenderUTF8_Solid(const char *text, Color fg) {
  return Surface(TTF_RenderUTF8_Solid(font_struct_, text, fg));
}

Surface Font::RenderUTF8_Shaded(const char *text, Color fg, Color bg) {
  return Surface(TTF_RenderUTF8_Shaded(font_struct_, text, fg, bg));
}

Surface Font::RenderUTF8_Blended(const char *text, Color fg) {
  return Surface(TTF_RenderUTF8_Blended(font_struct_, text, fg));
}

unsigned int Font::height() {
  return TTF_FontHeight(font_struct_);
}

unsigned int Font::line_skip() {
  return TTF_FontLineSkip(font_struct_);
}

Font::FontWrapper::FontWrapper() : font(nullptr) {}
Font::FontWrapper::FontWrapper(TTF_Font *font_) : font(font_) {}

Font::FontWrapper::~FontWrapper() {
  if (font) TTF_CloseFont(font);
}
