#ifndef GRAPH_MODELXPARSE_H_
#define GRAPH_MODELXPARSE_H_

#include <string>
#include <xparse/xdata.h>
#include "model.h"

namespace xparse {

Model LoadModel(std::istream &in, const std::string &name, const xparse::XFile &templates,
                const std::string &root_frame, const std::string &model_frame,
                const std::string &textures_path);

}

#endif // GRAPH_MODELXPARSE_H_
