#ifndef GRAPH_VECTOR3XPARSE_H_
#define GRAPH_VECTOR3XPARSE_H_

#include <xparse/xdata.h>
#include "vector3.h"

namespace xparse {

Vector3 LoadFromVector(const xparse::XDataValue::NodeData &data);

}

#endif // GRAPH_VECTOR3XPARSE_H_
