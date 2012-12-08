#include <sstream>
#include "surfacepainterfactory.h"
#include "texteditcontrol.h"

using namespace sdlobj;
using namespace std;

TextEditControl::TextEditControl() : cursor_(0) {
  translate_keysym_ = SDL::instance().translate_keysym();
  SDL::instance().set_translate_keysym(true);
}

TextEditControl::~TextEditControl() {
  SDL::instance().set_translate_keysym(translate_keysym_);
}

void TextEditControl::set_cursor(const unsigned cursor) {
  if (cursor_ != cursor) {
    if (cursor > label().size()) cursor_ = label().size();
    else cursor_ = cursor;
    Invalidate();
  }
}

void TextEditControl::set_caption(const std::string &caption) {
  caption_ = caption;
  Invalidate();
}

unsigned TextEditControl::preferred_width() {
  unsigned my_width = CachedLabelControl::preferred_width();
  unsigned c_width;
  font().SizeUTF8(caption_.data(), &c_width, 0);
  return my_width + c_width;
}

void TextEditControl::OnEntered() {}

bool TextEditControl::ProcessKeyDown(const SDL_KeyboardEvent &event) {
  switch (event.keysym.sym) {
    case SDLK_BACKSPACE: {
      if (cursor_ > 0) {
        auto bi = label().begin() + --cursor_;
        label().erase(bi, bi + 1);
        Invalidate();
      }
      return true;
    }
    case SDLK_DELETE: {
      if (cursor_ < label().size()) {
        auto bi = label().begin() + cursor_;
        label().erase(bi, bi + 1);
        Invalidate();
      }
      return true;
    }
    case SDLK_LEFT:
      if (cursor_ > 0) {
        --cursor_;
        Invalidate();
      }
      return true;
    case SDLK_RIGHT:
      if (cursor_ < label().size()) {
        ++cursor_;
        Invalidate();
      }
      return true;
    case SDLK_RETURN:
      OnEntered();
      return true;
    default: {
      if (event.keysym.unicode) {
        char chr = (char)event.keysym.unicode;
        auto bi = label().begin() + cursor_++;
        label().insert(bi, chr);
        Invalidate();
        return true;
      }
      break;
    }
  }
  return false;
}

void TextEditControl::set_label(const std::string &label) {
  CachedLabelControl::set_label(label);
  cursor_ = 0;
}

void TextEditControl::Repaint(sdlobj::Surface &surface) {
  unsigned cw;
  if (!caption_.empty()) {
    stringstream str;
    str << caption_ << ": " << label().substr(0, cursor_);
    font().SizeUTF8(str.str().data(), &cw, nullptr);
    str << label().substr(cursor_);
    surface = font().RenderUTF8_Blended(str.str().data(), font_color());
  } else {
    font().SizeUTF8(label().substr(0, cursor_).data(), &cw, nullptr);
    surface = font().RenderUTF8_Blended(label().data(), font_color());
  }

  if (surface.width() == 0) {
    surface = Surface(1, preferred_height());
  } else if (cw == surface.width()) {
    --cw;
  }
  SurfacePainterWrapper *painter = GetSurfacePainter(&surface);
  painter->StartDrawing();
  DrawLine(painter, cw, 0, cw, font().line_skip(), surface.ColorToPixel(font_color()));
  painter->FinishDrawing();
  delete painter;
}
