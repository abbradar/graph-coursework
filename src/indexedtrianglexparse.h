#ifndef GRAPH_INDEXEDTRIANGLEXPARSE_H_
#define GRAPH_INDEXEDTRIANGLEXPARSE_H_

#include <xparse/xdata.h>
#include "indexedtriangle.h"

namespace xparse {

IndexedTriangle LoadFromMeshFace(const xparse::XDataValue::NodeData &data);

}

#endif // GRAPH_INDEXEDTRIANGLEXPARSE_H_
