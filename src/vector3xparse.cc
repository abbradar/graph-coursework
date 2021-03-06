#include "common/exception.h"
#include "common/debug.h"
#include "vector3xparse.h"

using namespace xparse;

namespace xparse {

Vector3 LoadFromVector(const XDataValue::NodeData &data) {
  // this should be checked during .x template validation, so it's for debug only
#if DEBUG_LEVEL >= 4
  if (data.size() != 3) {
    throw Exception("Invalid Vector data");
  }
  for (auto &i : data) {
    if (i->type() != XDataValue::kFloat) {
      throw Exception("Invalid Vector data");
    }
  }
#endif
  return Vector3(data[0]->data().float_value,
                 data[1]->data().float_value,
                 data[2]->data().float_value);
}

}
