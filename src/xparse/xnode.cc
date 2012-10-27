#include "common/exception.h"
#include "common/debug.h"
#include "xdata.h"

using namespace xparse;
using namespace std;

XDataValue::XDataValue(XDataType type) : type_(type) {
  switch (type_) {
    case kInteger:
    case kFloat:
      break;
    case kString:
      data_.string_value = new string();
      break;
    case kNode:
      data_.node = new XNode();
      break;
    case kIntegerArray:
      data_.int_array = new vector<int>();
      break;
    case kFloatArray:
      data_.float_array = new vector<float>();
      break;
    case kStringArray:
      data_.string_array = new vector<string>();
      break;
    case kNodeArray:
      data_.node_array = new vector<XNode>();
      break;
    case kNodeReference:
      data_.node_reference = new XDataReference();
      break;
    default:
      throw Exception("Invalid data type");
  }
}

XDataValue::XDataValue(const XDataValue &other) {
  memset(&data, 0, sizeof(Data)); // for ensuring that all pointers in union are nullptr
  operator =(other);
}

XDataValue::~XDataValue() {
  FreeData();
}

void XDataValue::FreeData() {
  switch (type_) {
    case kInteger:
    case kFloat:
      break;
    case kString:
      delete data_.string_value;
      break;
    case kNode:
      delete data_.node;
      break;
    case kIntegerArray:
      delete data_.int_array;
      break;
    case kFloatArray:
      delete data_.float_array;
      break;
    case kStringArray:
      delete data_.string_array;
      break;
    case kNodeArray:
      delete data_.node_array;
      break;
    case kNodeReference:
      delete data_.node_reference;
      break;
    default:
      Assert(false);
      break;
  }
}

XDataValue &XDataValue::operator =(const XDataValue &other) {
  FreeData();
  type_ = other.type_;
  switch (type_) {
    case kInteger:
      data_.int_value = other.data_.int_value;
      break;
    case kFloat:
      data_.float_value = other.data_.float_value;
      break;
    case kString:
      data_.string_value = new string(other.data_.string_value);
      break;
    case kNode:
      data_.node = new XNode(other.data_.node);
      break;
    case kIntegerArray:
      data_.int_array = new vector<int>(other.data_.int_array);
      break;
    case kFloatArray:
      data_.float_array = new vector<float>(other.data_.float_array);
      break;
    case kStringArray:
      data_.string_array = new vector<string>(other.data_.string_array);
      break;
    case kNodeArray:
      data_.node_array = new vector<XNode>(other.data_.node_array);
      break;
    case kNodeReference:
      data_.node_reference = new XDataReference(other.data_.node_reference);
      break;
    default:
      Assert(false);
  }
  return *this;
}
