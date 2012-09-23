#ifndef GRAPH_FONTMANAGER_H_
#define GRAPH_FONTMANAGER_H_

#include <map>
#include <string>
#include "sdlobj/font.h"

class FontManager {
 public:
  sdlobj::Font GetFont(const std::string &&name, const int ptsize, const int index = 0);

 private:
  struct FontID {
    std::string name;
    int ptsize;
    int index;

    friend bool operator <(const FontID &a, const FontID &b);
    friend bool operator >(const FontID &a, const FontID &b);
    friend bool operator ==(const FontID &a, const FontID &b);
  };

  friend bool operator <(const FontID &a, const FontID &b);
  friend bool operator >(const FontID &a, const FontID &b);
  friend bool operator ==(const FontID &a, const FontID &b);

  std::map<FontID, sdlobj::Font> fonts_;
};

#endif // GRAPH_FONTMANAGER_H_
