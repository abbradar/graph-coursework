#ifndef GRAPH_MODELSSETTINGS_H_
#define GRAPH_MODELSSETTINGS_H_

#include <istream>
#include "xparse/xfile.h"
#include "common/settings.h"
#include "models.h"

namespace settings {

class ModelsLoader {
 public:
  ModelsLoader(Models *models);

  void Load(std::istream &in, std::string &textures_path);
  bool LoadTemplates(std::istream &in);

 private:
  Models *models_;
  xparse::XFile templates_;
};

class ModelsSettings : public SettingsBlock {
 public:
  ModelsSettings(Models *models);
  virtual ~ModelsSettings();

  inline Models *models() {
    return models_;
  }

  void set_models(Models *models);

  virtual const std::string name();
  virtual void operator <<(const YAML::Node &node);
 private:
  static const std::string kName;
  Models *models_;
};

}

#endif // GRAPH_MODELSSETTINGS_H_
