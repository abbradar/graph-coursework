#ifndef GRAPH_SCENE_H_
#define GRAPH_SCENE_H_

#include <iostream>
#include <map>
#include <string>
#include "common/settings.h"
#include "sdlobj/color.h"
#include "xparse/xfile.h"
#include "sceneobject.h"

class Scene {
 public:
  typedef std::vector<SceneObject> ObjectVector;

  Scene();

  inline ObjectVector &objects() {
    return objects_;
  }

  inline sdlobj::Color &plane_color() {
    return plane_color_;
  }

  void set_plane_color(const sdlobj::Color &plane_color);

  bool Load(std::istream &in, const std::string root_frame);
  //void Save(std::ostream &out);
  bool LoadTemplates(std::istream &in);

 private:
  ObjectVector objects_;
  sdlobj::Color plane_color_;
  xparse::XFile xfile_;
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
  static const char *const kName;
  Scene *scene_;
};

#endif // GRAPH_SCENE_H_
