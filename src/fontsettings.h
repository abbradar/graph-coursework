#ifndef GRAPH_FONTSETTINGS_H_
#define GRAPH_FONTSETTINGS_H_

#include <yaml-cpp/yaml.h>
#include "sdlobj/font.h"

void operator >>(const YAML::Node &node, sdlobj::Font &font);

#endif // GRAPH_FONTSETTINGS_H_
