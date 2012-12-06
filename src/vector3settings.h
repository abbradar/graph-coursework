#ifndef GRAPH_VECTOR3SETTINGS_H_
#define GRAPH_VECTOR3SETTINGS_H_

#include <yaml-cpp/yaml.h>
#include "vector3.h"

namespace settings {

void operator >>(const YAML::Node &node, Vector3 &vector);
void operator <<(YAML::Emitter &emitter, const Vector3 &vector);

}

#endif // GRAPH_VECTOR3SETTINGS_H_
