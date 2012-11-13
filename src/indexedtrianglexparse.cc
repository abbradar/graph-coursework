#include "common/debug.h"
#include "common/exception.h"
#include "indexedtrianglexparse.h"

using namespace xparse;

namespace xparse {

IndexedTriangle LoadFromMeshFace(const XDataValue::NodeData &data) {
  // this should be checked during .x template validation, so it's for debug only
#if DEBUG_LEVEL >= 4
  if (data.empty()) {
    throw Exception("Invalid MeshFace data");
  }
  for (auto &i : data) {
    if (i->type() != XDataValue::kInteger) {
      throw Exception("Invalid MeshFace data");
    }
  }
#endif
  if (data[0]->data().int_value != (int)IndexedTriangle::kPointsSize) {
    throw Exception("Unsupported number of indices in MeshFace");
  }
  IndexedTriangle p;
  XDataValue::ArrayData *indexes = data[1]->data().array_value;
  for (size_t i = 0; i < IndexedTriangle::kPointsSize; ++i) {
    p.points[i] = (*indexes)[i].int_value;
  }
  return p;
}

}
