#include "positionsettings.h"

namespace settings {

void operator >>(const YAML::Node &node, Position &position) {
  node[0] >> position.x;
  node[1] >> position.y;
  node[2] >> position.z;
  node[3] >> position.roll;
  node[4] >> position.pitch;
  node[5] >> position.yaw;
}

void operator <<(YAML::Emitter &emitter, const Position &position) {
  emitter << YAML::Flow << YAML::BeginSeq;
  emitter << position.x << position.y << position.z;
  emitter << position.roll << position.pitch << position.yaw;
  emitter << YAML::EndSeq;
}

}
