#ifndef GRAPH_MODELS_H_
#define GRAPH_MODELS_H_

#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include "xparse/xfile.h"
#include "common/settings.h"
#include "model.h"

class Models {
 public:
  typedef std::map<std::string, std::shared_ptr<Model>> ModelVector;

  Models();

  void Load(std::istream &in);
  bool LoadTemplates(std::istream &in);

  inline const ModelVector &models() const {
    return models_;
  }

 private:
  ModelVector models_;
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

#endif // GRAPH_MODELS_H_
