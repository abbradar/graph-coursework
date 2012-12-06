#ifndef GRAPH_LIGHTSOURCESETTINGS_H_
#define GRAPH_LIGHTSOURCESETTINGS_H_

#include <yaml-cpp/yaml.h>
#include "lightsource.h"

namespace settings {

void operator >>(const YAML::Node &node, LightSource &light_source);
void operator <<(YAML::Emitter &emitter, const LightSource &light_source);

}

#endif // GRAPH_LIGHTSOURCESETTINGS_H_
