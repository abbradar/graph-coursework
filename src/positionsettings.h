#ifndef GRAPH_POSITIONSETTINGS_H_
#define GRAPH_POSITIONSETTINGS_H_

#include <yaml-cpp/yaml.h>
#include "position.h"

namespace settings {

void operator >>(const YAML::Node &node, Position &position);
void operator <<(YAML::Emitter &emitter, const Position &position);

}

#endif // GRAPH_POSITIONSETTINGS_H_
