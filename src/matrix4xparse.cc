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
  Assert(array->size() == Matrix4::kMatrixWidth * Matrix4::kMatrixHeight);
  for (size_t i = 0; i < Matrix4::kMatrixWidth * Matrix4::kMatrixHeight; ++i) {
    matrix.data()[i] = (*array)[i].float_value;
  }
  return matrix.Transpose();
}

}
