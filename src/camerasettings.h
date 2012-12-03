#ifndef GRAPH_CAMERASETTINGS_H_
#define GRAPH_CAMERASETTINGS_H_

#include <yaml-cpp/yaml.h>
#include "camera.h"

namespace settings {

void operator >>(const YAML::Node &node, Camera &camera);

}

#endif // GRAPH_CAMERASETTINGS_H_
