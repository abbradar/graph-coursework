#ifndef GRAPH_COLORXPARSE_H_
#define GRAPH_COLORXPARSE_H_

#include <xparse/xdata.h>
#include "sdlobj/color.h"
#include "vector3.h"

namespace xparse {

sdlobj::Color LoadFromColorRGB(const xparse::XDataValue::NodeData &data);
sdlobj::Color LoadFromColorRGBA(const xparse::XDataValue::NodeData &data);

Vector3 LoadFloatFromColorRGB(const xparse::XDataValue::NodeData &data);
Vector3 LoadFloatFromColorRGBA(const xparse::XDataValue::NodeData &data);

}

#endif // GRAPH_COLORXPARSE_H_
