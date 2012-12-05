#include "common/debug.h"
#include "common/exception.h"
#include "matrix4xparse.h"

namespace xparse {

Matrix4 LoadFromTransformMatrix(xparse::XData *data) {
  if (data->template_id != "FrameTransformMatrix") {
    throw Exception("FrameTransformMatrix type expected.");
  }
  Matrix4 matrix;
  XDataValue *matrix_node = (*data->data[0]->data().node_value)[0].get();
  auto array = matrix_node->data().array_value;
  Assert(array->size() == (size_t)matrix.rows() * matrix.cols());
  for (size_t i = 0; i < (size_t)matrix.rows() * matrix.cols(); ++i) {
    matrix.data()[i] = (*array)[i].float_value;
  }
  return matrix;
}

}
