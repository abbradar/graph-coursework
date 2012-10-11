#include "indexedtriangle.h"

IndexedTriangle::IndexedTriangle()
  : IndexedTriangle(0, 0, 0) {}

IndexedTriangle::IndexedTriangle(int *points)
  : IndexedTriangle(points[0], points[1], points[2]) {}

IndexedTriangle::IndexedTriangle(int v1, int v2, int v3) {
  Set(v1, v2, v3);
}
