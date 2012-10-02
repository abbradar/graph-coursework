#ifndef GRAPH_INDEXEDTRIANGLE_H_
#define GRAPH_INDEXEDTRIANGLE_H_

struct IndexedTriangle {
  static constexpr int kPointsSize = 3;

  int points[kPointsSize];

  IndexedTriangle();
  IndexedTriangle(int v1, int v2, int v3);
  IndexedTriangle(int *points);

  inline void Set(int v1, int v2, int v3) {
    points[0] = v1;
    points[1] = v2;
    points[2] = v3;
  }
};

#endif // GRAPH_INDEXEDTRIANGLE_H_
