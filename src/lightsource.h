#ifndef GRAPH_LIGHTSOURCE_H_
#define GRAPH_LIGHTSOURCE_H_

#include "vector3.h"
#include "myfloat.h"

struct LightSource {
  myfloat specular;
  myfloat diffuse;
};

struct FullLightSource : public LightSource {
  Vector3 position;
};

#endif // GRAPH_LIGHTSOURCE_H_
