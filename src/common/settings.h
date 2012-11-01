#ifndef COMMON_SETTINGS_H_
#define COMMON_SETTINGS_H_

#include <memory>
#include <iostream>
#include <string>
#include <map>
#include <yaml-cpp/yaml.h>

class SettingsBlock {
 public:
  SettingsBlock();
  virtual ~SettingsBlock();

  virtual const std::string name() = 0;
  virtual void operator <<(const YAML::Node &node) = 0;
  //virtual void operator >>(YAML::Node &node) = 0;
};

class Settings {
 public:
  typedef std::map<std::string, std::shared_ptr<SettingsBlock>> SettingsMap;

  Settings();

  void LoadSettings(std::istream &file);
  void SaveSettings(std::ostream &file);

  const SettingsMap &settings() const {
    return settings_;
  }

  void AddBlock(std::shared_ptr<SettingsBlock> block);
  void RemoveBlock(const std::string name);

 private:
  SettingsMap settings_;
};

#endif // COMMON_SETTINGS_H_
