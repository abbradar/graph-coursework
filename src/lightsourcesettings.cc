#include "vector3settings.h"
#include "lightsourcesettings.h"

namespace settings {

void operator >>(const YAML::Node &node, FullLightSource &light_source) {
  node["position"] >> light_source.position;
  node["specular"] >> light_source.specular;
  node["diffuse"] >> light_source.diffuse;
}

void operator <<(YAML::Emitter &emitter, const FullLightSource &light_source) {
  emitter << YAML::BeginMap;
  emitter << YAML::Key << "position" << YAML::Value << light_source.position;
  emitter << YAML::Key << "specular"  << YAML::Value << light_source.specular;
  emitter << YAML::Key << "diffuse"  << YAML::Value << light_source.diffuse;
  emitter << YAML::EndMap;
}

}
