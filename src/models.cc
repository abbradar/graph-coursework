#include <fstream>
#include <yaml-cpp/yaml.h>
#include "models.h"

using namespace std;

Models::Models() = default;

void Models::Load(istream &in) {
  YAML::Parser parser(in);
  YAML::Node doc;
  parser.GetNextDocument(doc);

  for (auto &mi : doc) {
    shared_ptr<Model> model = make_shared<Model>(Model::LoadFromYaml(mi, templates_));
    const string &name = model->name();
    models_.insert(pair<string, shared_ptr<Model>>(name, move(model)));
  }
}

bool Models::LoadTemplates(std::istream &in) {
  return templates_.Parse(in);
}

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
  std::string x_templates_name;
  node["templates"] >> x_templates_name;
  std::ifstream x_templates(x_templates_name);
  if (!models_->LoadTemplates(x_templates)) {
    throw Exception("Cannot load .x templates.");
  }
  x_templates.close();
  std::string x_models_name;
  node["models"] >> x_models_name;
  std::ifstream x_models(x_models_name);

  models_->Load(x_models);
  x_models.close();
}
