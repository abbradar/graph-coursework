#include "common/exception.h"
#include "common/debug.h"
#include "xdata.h"

using namespace xparse;
using namespace std;

bool XDataReference::Resolve(XFile *file) {
  if (!id.empty()) {
    for (auto &i : file->data_nodes()) {
      if (i->id == id) {
        ptr = i.get();
        return true;
      }
    }
    return false;
  } else {
    for (auto &i : file->data_nodes()) {
      if (i->guid == guid) {
        ptr = i.get();
        return true;
      }
    }
    return false;
  }
}

XNestedData::XNestedData(Type type) : type_(type) {
  switch (type_) {
      break;
    case kString:
      data_.node = new string();
    case kNode:
      data_.node = new XNode();
      break;
    case kNodeReference:
      data_.reference = new XNodeReference();
      break;
    default:
      throw Exception("Invalid nested data type");
  }
}

XNestedData::XNestedData(const XNestedData &other) {
  memset(data_, 0, sizeof(Data));
  operator =(other);
}

XNestedData::~XNestedData() {
  FreeData();
}

void XNestedData::FreeData() {
  switch (type_) {
    case kNode:
      delete data_.node;
      break;
    case kNodeReference:
      delete data_.reference;
      break;
    default:
      Assert(false);
      break;
  }
}

XNestedData &XNestedData::operator =(const XNestedData &other) {
  FreeData();
  type_ = other.type_;
  switch (type_) {
    case kNode:
      data_.node = new XNode(other.data_.node);
      break;
    case kNodeReference:
      data_.reference = new XNodeReference(other.data_.reference);
      break;
    default:
      Assert(false);
      break;
  }
  return *this;
}

XDataValue::XDataValue(Type type, bool array_type) : type_(type), array_type_(array_type) {
  if (array_type_) {
    data_.array_value = new ArrayData();
  } else {
    switch (type_) {
      case kInteger:
      case kFloat:
        break;
      case kString:
        data_.string_value = new string();
        break;
      case kNode:
        data_.nested_value = new NestedData();
        break;
      default:
        throw Exception("Invalid data type");
    }
  }
}

XDataValue::XDataValue(const XDataValue &other) {
  memset(&data_, 0, sizeof(Data)); // for ensuring that all pointers in union are nullptr
  operator =(other);
}

XDataValue::~XDataValue() {
  FreeData();
}

void XDataValue::FreeData() {
  if (array_type_) {
    delete data.array_type;
  } else {
    switch (type_) {
      case kInteger:
      case kFloat:
        break;
      case kString:
        delete data_.string_value;
        break;
      case kNode:
        delete data_.nested_value;
        break;
      default:
        Assert(false);
        break;
    }
  }
}

XDataValue &XDataValue::operator =(const XDataValue &other) {
  FreeData();
  type_ = other.type_;
  array_type_ = other.array_type_;
  if (array_type_) {
    data_.array_value = new ArrayData(*(other.data_.array_value));
  } else {
    switch (type_) {
      case kInteger:
        data_.int_value = other.data_.int_value;
        break;
      case kFloat:
        data_.float_value = other.data_.float_value;
        break;
      case kString:
        data_.string_value = new string(*(other.data_.string_value));
        break;
      case kNode:
        data_.nested_value = new NestedData(*(other.data_.nested_value));
      default:
        Assert(false);
    }
  }
  return *this;
}

bool ValidateData(XTemplateMember &member, vector<XDataValue>::iterator &in,
                  vector<XDataValue>::iterator &end, Data &out) {
  switch (member.member_type()) {
    case kInteger:
      if (in->array_value()) return false;
      if (in->type() == kInteger) out.int_value = in->data().int_value;
      else if (in->type() == kFloat) out.int_value = (int)in->data().float_value;
      else return false;
      ++in;
      break;
    case kFloat;
      if (in->array_value()) return false;
      value = new XDataValue(kInteger);
      if (in->type() == kInteger) out.float_value = in->data().int_value;
      else if (in->type() == kFloat) out.float_value = in->data().float_value;
      else return false;
      ++in;
      break;
    case kString:
      if (in->array_value()) return false;
      if (in->type() != kString) return false;
      *(out.string_value) = *(in->data().string_value);
      ++in;
      break;
    case kNode:
      if (!ValidateTemplate(member.template_reference()->ptr, in, end, *(data.nested_value))
        return false;
      break;
    default:
      Assert(false);
  }
  return true;
}

bool ValidateTemplate(const XTemplate *nested, vector<XDataValue>::iterator &in,
                      vector<XDataValue>::iterator &end, vector<XDataValue> &out) {
  for (auto &member : nested->members) {
    if (in == end) return false;
    XDataValue::Type type;
    switch (member.basic_type()) {
      case kInteger:
        type = kInteger;
        break;
      case kFloat:
        type = kFloat;
        break;
      case kString:
        type = kString;
        break;
      case kNode:
        type = kNode;
        break;
    }
    XDataValue value(type, member.member_type() == kArray);
    if (value.array_type()) {
      int size = 1;
      for (auto &i : *(member.array_size())) size *= i;
      for (int i = 0; i < size; ++i) {
        XDataValue::Data data;
        if (!ValidateData(member, in, end, data)) return false;
        value.data().array_value->push_back(data);
      }
    } else {
      if (!ValidateData(member, in, end, value.data())) return false;
    }

    out.push_back(move(value));
  }
  return true;
}

bool XData::Validate(XFile *file) {
  auto ti = file->data_nodes().find(template_id);
  if (ti == file->data_nodes().end()) return false;
  template_ptr = ti->get();
  vector<XDataValue> new_data;
  if (!ValidateTemplate(template_ptr, data->begin(), data->end(), new_data)) return false;
  
  switch (template_ptr->restriction_type) {
    case kClosed:
      if (!nested_data.empty()) return false;
      break;
    case kRestricted:
      for (auto &i : nested_data) {
        bool found = false;
        switch (i.type()) {
          case kNode:
            for (auto &j : template_ptr->restrictions) {
              if (j.name == i.data().node->template_id) {
                found = true;
                break;
              }
            }
            break;
          case kReference:
            // TODO: not supported
            return false;
          default:
            Assert(false);
        }
        if (!found) return false;
      }
    case kOpened:
      break;
    default:
      Assert(false);
      break;
  }

  return true;
}
