#ifndef GRAPH_LIGHTSOURCE_H_
#define GRAPH_LIGHTSOURCE_H_

#include "vector3.h"
#include "myfloat.h"

struct LightSource {
  Vector3 position;
  myfloat specular;
  myfloat diffuse;
};

#endif // GRAPH_LIGHTSOURCE_H_
