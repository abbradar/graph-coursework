#ifndef GRAPH_SETTINGS_H_
#define GRAPH_SETTINGS_H_

class Settings {
 public:
  Settings();

  void LoadSettings(const char *file);
  void SaveSettings(const char *file);


};

#endif // GRAPH_SETTINGS_H_
