#include <cstring>
#include "common/exception.h"
#include "common/debug.h"
#include "xtemplate.h"

using namespace xparse;
using namespace std;

bool XTemplateReference::Resolve(XFile *file) {
  if (!id.empty()) {
    auto i = file->templates().find(id);
    if (i == file->templates().begin()) return false;
    ptr = i->second().get();
    return true;
  } else {
    for (auto i : file->templates()) {
      if (i->second()->guid == guid) {
        ptr = i->second().get();
        return true;
      }
    }
    return false;
  }
}

XTemplateMember::XTemplateMember(MemberType member_type, BasicType basic_type) {
  member_type_ = member_type;
  basic_type_ = basic_type;
  switch (member_type_) {
    case kBasic:
      break;
    case kArray:
      array_size.reset(new vector<size_t>());
    default:
      throw Exception("Invalid member type");
  }
  switch (basic_type_) {
    case kInteger:
    case kFloat:
    case kString:
      break;
    case kNode:
      template_reference_.reset(new XTemplateReference());
      break;
    default:
      throw Exception("Invalid basic type");
  }
}

XTemplateMember::XTemplateMember(const XTemplateMember &other) {
  operator =(other);
}

XTemplateMember &XTemplateMember::operator =(const XTemplateMember &other) {
  id_ = other.id_;
  member_type_ = other.member_type_;
  basic_type_ = other.basic_type_;
  switch (member_type_) {
    case kBasic:
      break;
    case kArray:
      array_size.reset(new vector<size_t>(*(other.array_size_)));
      break;
    default:
      Assert(false);
      break;
  }
  switch (basic_type_) {
    case kInteger:
    case kFloat:
    case kString:
      break;
    case kNode:
      template_reference_.reset(new XTemplateReference());
      break;
    default:
      Assert(false);
      break;
  }

  return *this;
}
