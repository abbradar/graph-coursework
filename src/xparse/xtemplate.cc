#include <cstring>
#include "common/exception.h"
#include "common/debug.h"
#include "xtemplate.h"

using namespace std;

namespace xparse {

bool XTemplateReference::Resolve(XFile *file) {
  if (!id.empty()) {
    auto i = file->templates().find(id);
    if (i == file->templates().end()) return false;
    ptr = i->second.get();
    return true;
  } else {
    for (auto &i : file->templates()) {
      if (i.second->guid == guid) {
        ptr = i.second.get();
        return true;
      }
    }
    return false;
  }
}

bool XTemplateMemberReference::Resolve(XTemplate *x_template) {
  if (id.empty()) return false;
  bool found = false;
  for (size_t i = 0; i < x_template->members.size(); ++i) {
    if (x_template->members[i]->id() == id) {
      ptr = x_template->members[i].get();
      pos = i;
      found = true;
      break;
    }
  }
  return found;
}

XTemplateMember::ArraySize::ArraySize(const Type type) : type_(type) {
  switch (type_) {
    case kStatic:
      break;
    case kDynamic:
      data_.reference = new XTemplateMemberReference();
      break;
    default:
      Assert(false);
      break;
  }
}

XTemplateMember::ArraySize::ArraySize(const Type type, void *ptr) : type_(type) {
  switch (type_) {
    case kStatic:
      //data_.size = *(size_t *)ptr;
      break;
    case kDynamic:
      data_.reference = (XTemplateMemberReference *)ptr;
      break;
    default:
      Assert(false);
      break;
  }
}

XTemplateMember::ArraySize::ArraySize(const ArraySize &other) : type_(kStatic) {
  memset(&data_, 0, sizeof(Data));
  operator =(other);
}

XTemplateMember::ArraySize::~ArraySize() {
  FreeData();
}

void XTemplateMember::ArraySize::FreeData() {
  switch (type_) {
    case kStatic:
      break;
    case kDynamic:
      delete data_.reference;
      break;
    default:
      Assert(false);
  }
}

XTemplateMember::ArraySize &XTemplateMember::ArraySize::operator =(const ArraySize &other) {
  FreeData();
  type_ = other.type_;
  switch (type_) {
    case kStatic:
      data_.size = other.data_.size;
      break;
    case kDynamic:
      data_.reference = new XTemplateMemberReference(*(other.data_.reference));
      break;
    default:
      Assert(false);
  }

  return *this;
}

XTemplateMember::XTemplateMember(MemberType member_type, BasicType basic_type) {
  member_type_ = member_type;
  basic_type_ = basic_type;
  switch (member_type_) {
    case kBasic:
      break;
    case kArray:
      array_size_.reset(new vector<ArraySize>());
      break;
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

XTemplateMember::XTemplateMember(const XTemplateMember &other) : member_type_(kBasic), basic_type_(kInteger) {
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
      array_size_.reset(new vector<ArraySize>(*(other.array_size_)));
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

XTemplateMember::~XTemplateMember() = default;

}
