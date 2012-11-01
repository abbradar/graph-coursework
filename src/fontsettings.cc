#include "fontsettings.h"

using namespace sdlobj;

void operator >>(const YAML::Node &node, Font &font) {
  std::string font_name;
  node["name"] >> font_name;
  int font_size;
  node["size"] >> font_size;
  int font_index;
  node["index"] >> font_index;

  font = Font(font_name.data(), font_size, font_index);
}
