#include <fstream>
#include <boost/filesystem.hpp>
#include "modelxparse.h"
#include "modelsettings.h"

using namespace xparse;
using namespace boost::filesystem;

namespace settings {

Model LoadModel(const YAML::Node &node, const XFile &templates,
                const std::string &textures_path) {
  std::string name;
  node["name"] >> name;
  std::string root_frame;
  node["root-frame"] >> root_frame;
  std::string model_frame;
  node["model-frame"] >> model_frame;
  std::string x_model;
  node["model"] >> x_model;
  bool lighting;
  node["lighting"] >> lighting;
  std::ifstream in(x_model);
  path curr_path = current_path();
  path new_path = absolute(path(x_model).parent_path());
  current_path(new_path);
  Model model = xparse::LoadModel(in, name, templates, root_frame, model_frame,
                                  textures_path, lighting);
  in.close();
  current_path(curr_path);
  return model;
}

}
