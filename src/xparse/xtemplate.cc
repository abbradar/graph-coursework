#include <cstring>
#include "common/exception.h"
#include "common/debug.h"
#include "xtemplate.h"

using namespace xparse;
using namespace std;

XTemplateMember::XTemplateMember(MemberType type) : type_(type) {
  switch (type_) {
    case kBasic:
      break;
    case kArray:
      data_.array_type.size = new vector<size_t>();
      break;
    case kTemplate:
      data_.template_type = new string();
      break;
    default:
      throw Exception("Invalid member type");
  }
}

XTemplateMember::XTemplateMember(const XTemplateMember &other) {
  memset(&data_, 0, sizeof(TypeData)); // for ensuring that all pointers in union are nullptr
  operator =(other);
}

XTemplateMember::~XTemplateMember() {
  FreeData();
}

void XTemplateMember::FreeData() {
  switch (type_) {
    case kBasic:
      break;
    case kArray:
      delete data_.array_type.size;
      break;
    case kTemplate:
      delete data_.template_type;
      break;
    default:
      Assert(false);
      break;
  }
}

XTemplateMember &XTemplateMember::operator =(const XTemplateMember &other) {
  FreeData();
  id_ = other.id_;
  type_ = other.type_;
  switch (type_) {
    case kBasic:
      data_.basic_type = other.data_.basic_type;
      break;
    case kArray:
      data_.array_type.basic_type = other.data_.array_type.basic_type;
      data_.array_type.size = new vector<int>(other.data_.array_type.size);
      break;
    case kTemplate:
      data_.template_type = new string(other.data_.template_type);
      break;
    default:
      Assert(false);
      break;
  }
  return *this;
}
