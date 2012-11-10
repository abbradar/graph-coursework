#ifndef GRAPH_SCENE_H_
#define GRAPH_SCENE_H_

#include <iostream>
#include <list>
#include <memory>
#include <string>
#include "common/settings.h"
#include "sdlobj/color.h"
#include "xparse/xfile.h"
#include "sceneobject.h"
#include "models.h"

class Scene {
 public:
  typedef std::list<std::shared_ptr<SceneObject>> ObjectList;

  Scene();

  inline ObjectList &objects() {
    return objects_;
  }

  inline sdlobj::Color &plane_color() {
    return plane_color_;
  }

  void set_plane_color(const sdlobj::Color &plane_color);

  void Load(std::istream &in, const Models &models);
  void Save(std::ostream &out);

 private:
  ObjectList objects_;
  sdlobj::Color plane_color_;
};

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

#endif // GRAPH_SCENE_H_
