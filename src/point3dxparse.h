#ifndef GRAPH_POINT3DXPARSE_H_
#define GRAPH_POINT3DXPARSE_H_

#include <xparse/xdata.h>
#include "point3d.h"

namespace xparse {

Point3D LoadFromVector(const xparse::XDataValue::NodeData &data);

}

#endif // GRAPH_POINT3DXPARSE_H_
