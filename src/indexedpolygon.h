#ifndef GRAPH_INDEXEDPOLYGON_H_
#define GRAPH_INDEXEDPOLYGON_H_

struct IndexedPolygon {
  int v1;
  int v2;
  int v3;

  IndexedPolygon();
  IndexedPolygon(int v1, int v2, int v3);
};

#endif // GRAPH_INDEXEDPOLYGON_H_
