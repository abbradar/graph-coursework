#ifndef GRAPH_COLORSETTINGS_H_
#define GRAPH_COLORSETTINGS_H_

#include <yaml-cpp/yaml.h>
#include "sdlobj/color.h"

void operator >>(const YAML::Node &node, sdlobj::Color &color);

#endif // GRAPH_COLORSETTINGS_H_
