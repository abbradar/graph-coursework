#include "common/exception.h"
#include "common/debug.h"
#include "vector2xparse.h"

using namespace xparse;

namespace xparse {

Vector2 LoadFromCoords2D(const xparse::XDataValue::NodeData &data) {
  // this should be checked during .x template validation, so it's for debug only
#if DEBUG_LEVEL >= 4
  if (data.size() != 2) {
    throw Exception("Invalid Coords2d data");
  }
  for (auto &i : data) {
    if (i->type() != XDataValue::kFloat) {
      throw Exception("Invalid Coords2d data");
    }
  }
#endif
  return Vector2(data[0]->data().float_value, data[1]->data().float_value);
}

}
