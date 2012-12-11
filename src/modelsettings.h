#ifndef GRAPH_MODELSETTINGS_H_
#define GRAPH_MODELSETTINGS_H_

#include <yaml-cpp/yaml.h>
#include "xparse/xfile.h"
#include "model.h"

namespace settings {

Model LoadModel(const YAML::Node &node, const xparse::XFile &templates,
                const std::string &textures_path);

}

#endif // GRAPH_MODELSETTINGS_H_
