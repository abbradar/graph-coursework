#define BOOST_FILESYSTEM_NO_DEPRECATED

#include <fstream>
#include <string>
#include <boost/filesystem.hpp>
#include "common/exception.h"
#include "modelsettings.h"
#include "modelssettings.h"

using namespace std;
using namespace boost::filesystem;

namespace settings {

ModelsSettings::ModelsSettings(Models *models) : models_(models) {}

ModelsSettings::~ModelsSettings() = default;

const std::string ModelsSettings::kName = std::string("models");

const std::string ModelsSettings::name() {
  return kName;
}

void ModelsSettings::set_models(Models *models) {
  models_ = models;
}

void ModelsSettings::operator <<(const YAML::Node &node) {
  ModelsLoader loader(models_);
  std::string x_templates_name;
  node["templates"] >> x_templates_name;
  std::ifstream x_templates(x_templates_name);
  if (!loader.LoadTemplates(x_templates)) {
    throw Exception("Cannot load .x templates.");
  }
  x_templates.close();
  std::string x_models_name;
  node["models"] >> x_models_name;
  std::ifstream x_models(x_models_name);

  path curr_path = current_path();
  path new_path = absolute(path(x_models_name).parent_path());
  current_path(new_path);
  loader.Load(x_models);
  x_models.close();
  current_path(curr_path);
}

ModelsLoader::ModelsLoader(Models *models) : models_(models) {}

void ModelsLoader::Load(istream &in) {
  YAML::Parser parser(in);
  YAML::Node doc;
  parser.GetNextDocument(doc);

  for (auto &mi : doc) {
    shared_ptr<Model> model = make_shared<Model>(LoadModel(mi, templates_));
    const string &name = model->name();
    models_->models().insert(pair<string, shared_ptr<Model>>(name, move(model)));
  }
}

bool ModelsLoader::LoadTemplates(std::istream &in) {
  return templates_.Parse(in);
}

}
