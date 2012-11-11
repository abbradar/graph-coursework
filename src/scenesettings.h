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

class SceneSettings : public SettingsBlock {
 public:
  SceneSettings(Scene *scene);
  virtual ~SceneSettings();

  inline Scene *scene() {
    return scene_;
  }

  void set_scene(Scene *scene);

  virtual const std::string name();
  virtual void operator <<(const YAML::Node &node);
 private:
  static const std::string kName;
  Scene *scene_;
};

}

#endif // GRAPH_SCENESETTINGS_H_
