#ifndef XPARSE_XDATA_H_
#define XPARSE_XDATA_H_

#include <string>
#include <vector>
#include "guid.h"

namespace xparse {

struct XNode;

struct XDataReference {
  std::string id;
  GUID guid;
};

class XDataValue {
 public:
  enum DataType {
    kInteger, kFloat, kString, kNode,
    kIntegerArray, kFloatArray, kStringArray, kNodeArray,
    kNodeReference
  };

  union Data {
    int int_value;
    float float_value;
    std::string *string_value;
    XNode *node;
    std::vector<int> *int_array;
    std::vector<float> *float_array;
    std::vector<std::string> *string_array;
    std::vector<XNode> *node_array;
    XDataReference *node_reference;
  };

  XDataValue(XDataType type);
  XDataValue(const XDataValue &other);
  ~XDataValue();

  XDataValue &operator =(const XDataValue &other);

  inline DataType type() {
    return type_;
  }

  inline Data &data() {
    return data_;
  }

 private:
  void FreeData();

  DataType type_;
  Data data_;
};

struct XNode {
  std::string template_id;
  std::string id;
  GUID guid;
  std::vector<XDataValue> data;
};

}

#endif // XPARSE_XDATA_H_
