#include "common/exception.h"
#include "indexedtriangle.h"

using namespace xparse;

IndexedTriangle IndexedTriangle::LoadFromXMeshFace(const XDataValue::NodeData &data) {
  // this should be checked during .x template validation, so it's for debug only
#if DEBUG_LEVEL == 4
  if (data.empty()) {
    throw Exception("Invalid MeshFace data");
  }
  for (auto &i : data) {
    if (i->type() != XDataValue::kInteger) {
      throw Exception("Invalid MeshFace data");
    }
  }
#endif
  if (data[0]->data().int_value != (int)kPointsSize) {
    throw Exception("Unsupported number of indices in MeshFace");
  }
  IndexedTriangle p;
  XDataValue::ArrayData *indexes = data[1]->data().array_value;
  for (size_t i = 0; i < kPointsSize; ++i) {
    p.points[i] = (*indexes)[i].int_value;
  }
  return p;
}

IndexedTriangle::IndexedTriangle()
  : IndexedTriangle(0, 0, 0) {}

IndexedTriangle::IndexedTriangle(int *points)
  : IndexedTriangle(points[0], points[1], points[2]) {}

IndexedTriangle::IndexedTriangle(int v1, int v2, int v3) {
  Set(v1, v2, v3);
}
