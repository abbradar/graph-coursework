#ifndef GRAPH_POLYGON_H_
#define GRAPH_POLYGON_H_

#include <string>

struct Point {
  float x;
  float y;
  float z;

  std::string Serialize();
  static Point Deserialize(std::string str);
};

struct Polygon {
  Point p1;
  Point p2;
  Point p3;

  std::string Serialize();
  static Point Deserialize(std::string str);
};

#endif // GRAPH_POLYGON_H_
