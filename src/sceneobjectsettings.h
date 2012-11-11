#ifndef GRAPH_SCENEOBJECTSETTINGS_H_
#define GRAPH_SCENEOBJECTSETTINGS_H_

#include <yaml-cpp/yaml.h>
#include "sceneobject.h"

namespace settings {

SceneObject LoadSceneObject(const YAML::Node &node, const Models &models);
void operator <<(YAML::Emitter &emitter, const SceneObject &scene_object);

}

#endif // GRAPH_SCENEOBJECTSETTINGS_H_
