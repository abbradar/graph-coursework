#include <fstream>
#include "fontsettings.h"

using namespace std;
using namespace sdlobj;

namespace settings {

void operator >>(const YAML::Node &node, Font &font) {
  std::string font_name;
  node["name"] >> font_name;
  int font_size;
  node["size"] >> font_size;
  int font_index;
  node["index"] >> font_index;

  font = Font(unique_ptr<istream>(new ifstream(font_name)), font_size, font_index);
}

}
