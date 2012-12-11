#ifndef GRAPH_MATERIALXPARSE_H_
#define GRAPH_MATERIALXPARSE_H_

#include <xparse/xdata.h>
#include "material.h"

namespace xparse {

Material LoadFromMaterial(const XData *data, const std::string &textures_path);

}

#endif // GRAPH_MATERIALXPARSE_H_
