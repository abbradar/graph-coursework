#include <boost/filesystem.hpp>
#include <string>
#include <fstream>
#include "common/exception.h"
#include "common/debug.h"
#include "sdlobj/sdlimage.h"
#include "colorxparse.h"
#include "config.h"
#include "material.h"

using namespace std;
using namespace xparse;
using namespace sdlobj;
using namespace boost::filesystem;

namespace xparse {

Material LoadFromMaterial(const XData *data, const string &textures_path) {
  // this should be checked during .x template validation, so it's for debug only
#if DEBUG_LEVEL >= 4
  if (data->template_id != "Material") {
    throw Exception("Invalid Material node");
  }
#endif
  Material material;
  material.set_ambient_color(LoadFloatFromColorRGBA(*(data->data[0]->data().node_value)));
  material.set_shininess(data->data[1]->data().float_value);
  material.set_specular_color(LoadFloatFromColorRGB(*(data->data[2]->data().node_value)));
  material.set_diffuse_color(LoadFloatFromColorRGB(*(data->data[3]->data().node_value)));
  for (auto &i : data->nested_data) {
    Assert(i.type() == XNestedData::kNode);
    XData *texture = i.data().node;
    if (texture->template_id == "TextureFilename") {
      path texture_file(*(texture->data[0]->data().string_value));
      string filename = (path(textures_path).remove_leaf() /= texture_file).string();
      FileSurfaceLoader *texture = new FileSurfaceLoader();
      texture->set_path(filename);
      material.set_texture_loader(std::shared_ptr<FileSurfaceLoader>(texture));
    }
  }
  return material;
}

}
