#include "vector3settings.h"

namespace settings {

void operator >>(const YAML::Node &node, Vector3 &vector) {
  node[0] >> vector.x();
  node[1] >> vector.y();
  node[2] >> vector.z();
}

void operator <<(YAML::Emitter &emitter, const Vector3 &vector) {
  emitter << YAML::Flow << YAML::BeginSeq;
  emitter << vector.x() << vector.y() << vector.z();
  emitter << YAML::EndSeq;
}

}
