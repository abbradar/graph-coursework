#ifndef GRAPH_MODELS_H_
#define GRAPH_MODELS_H_

#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include "xparse/xfile.h"
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

  inline ModelVector &models() {
    return models_;
  }

 private:
  ModelVector models_;
};

#endif // GRAPH_MODELS_H_
