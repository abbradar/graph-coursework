#include "common/exception.h"
#include "common/debug.h"
#include "colorxparse.h"

using namespace xparse;
using namespace sdlobj;

namespace xparse {

Color LoadFromColorRGB(const XDataValue::NodeData &data) {
  // this should be checked during .x template validation, so it's for debug only
#if DEBUG_LEVEL >= 4
  if (data.size() != 3) {
    throw Exception("Invalid ColorRGB data");
  }
  for (auto &i : data) {
    if (i->type() != XDataValue::kFloat) {
      throw Exception("Invalid ColorRGB data");
    }
  }
#endif
  return Color(data[0]->data().float_value * 256, data[1]->data().float_value * 256,
      data[2]->data().float_value * 256);
}

Color LoadFromColorRGBA(const XDataValue::NodeData &data) {
  // this should be checked during .x template validation, so it's for debug only
#if DEBUG_LEVEL >= 4
  if (data.size() != 4) {
    throw Exception("Invalid ColorRGBA data");
  }
  for (auto &i : data) {
    if (i->type() != XDataValue::kFloat) {
      throw Exception("Invalid ColorRGBA data");
    }
  }
#endif
  return Color(data[0]->data().float_value * 255, data[1]->data().float_value * 255,
      data[2]->data().float_value * 255, data[3]->data().float_value * 255);
}

}
