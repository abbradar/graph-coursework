#include "camerasettings.h"

namespace settings {

void operator >>(const YAML::Node &node, Camera &camera) {
  myfloat viewer_distance;
  node["viewer-distance"] >> viewer_distance;
  camera.set_viewer_distance(viewer_distance);
  myfloat scale;
  node["scale"] >> scale;
  camera.set_scale(scale);
}

}
