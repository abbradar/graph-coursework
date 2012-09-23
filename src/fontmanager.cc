#include "fontmanager.h"

using namespace sdlobj;

Font FontManager::GetFont(const std::string &&name, const int ptsize, const int index) {
  FontID id = { .name = name, .ptsize = ptsize, .index = index };

  auto i = fonts_.find(id);
  if (i == fonts_.end()) {
    Font new_font(name.data(), ptsize, index);
    fonts_.insert(std::pair<FontID, Font>(id, std::move(new_font)));
    return new_font;
  } else {
    return i->second;
  }
}

bool operator <(const FontManager::FontID &a, const FontManager::FontID &b)
{
  int result = a.name.compare(b.name);
  if (result < 0) return true;
  else if (result > 0) return false;
  if (a.index < b.index) return true;
  else if (a.index > b.index) return false;
  return a.ptsize < b.ptsize;
}

bool operator ==(const FontManager::FontID &a, const FontManager::FontID &b)
{
  if (a.name != b.name) return false;
  if (a.index != b.index) return false;
  return a.ptsize == b.ptsize;
}

bool operator >(const FontManager::FontID &a, const FontManager::FontID &b)
{
  int result = a.name.compare(b.name);
  if (result > 0) return true;
  else if (result < 0) return false;
  if (a.index > b.index) return true;
  else if (a.index < b.index) return false;
  return a.ptsize > b.ptsize;
}
