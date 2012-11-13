#ifndef GRAPH_SCENESETTINGS_H_
#define GRAPH_SCENESETTINGS_H_

#include <iostream>
#include <yaml-cpp/yaml.h>
#include "common/settings.h"
#include "models.h"
#include "scene.h"

namespace settings {

void LoadScene(Scene &scene, std::istream &in, const Models &models);
void SaveScene(Scene &scene, std::ostream &out);

}

#endif // GRAPH_SCENESETTINGS_H_
