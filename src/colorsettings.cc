#include <cstdio>
#include "common/exception.h"
#include "colorsettings.h"

namespace settings {

void operator >>(const YAML::Node &node, sdlobj::Color &color) {
  std::string color_code;
  node >> color_code;
  if (color_code.size() < 7) {
    throw Exception("Invalid color format.");
  }
  if (sscanf(color_code.data(), "#%2hhx%2hhx%2hhx", &color.r, &color.g, &color.b) == EOF) {
    throw Exception("Invalid color format.");
  }
}

}
