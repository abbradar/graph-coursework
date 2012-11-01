#include "common/debug.h"
#include "settings.h"

using namespace std;

SettingsBlock::SettingsBlock() = default;
SettingsBlock::~SettingsBlock() = default;

Settings::Settings() = default;

void Settings::LoadSettings(std::istream &file) {
  YAML::Parser parser(file);
  YAML::Node doc;
  parser.GetNextDocument(doc);
  for (auto &i : settings_) {
    *(i.second) << doc[i.first];
  }
}

void Settings::SaveSettings(std::ostream &file) {
  AssertMsg(false, "Not implemented");
}


void Settings::AddBlock(std::shared_ptr<SettingsBlock> block) {
  settings_.insert(pair<string, shared_ptr<SettingsBlock>>(block->name(), block));
}

void Settings::RemoveBlock(const std::string name) {
  settings_.erase(name);
}
