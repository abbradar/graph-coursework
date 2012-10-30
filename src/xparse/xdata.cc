#include "common/exception.h"
#include "common/debug.h"
#include "xdata.h"

using namespace std;

namespace xparse {

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
    case kNode:
      data_.node = new XData();
      break;
    case kNodeReference:
      data_.reference = new XDataReference();
      break;
    default:
      throw Exception("Invalid nested data type");
  }
}

XNestedData::XNestedData(const XNestedData &other) : type_(kNode) {
  memset(&data_, 0, sizeof(Data));
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
      data_.node = new XData(*(other.data_.node));
      break;
    case kNodeReference:
      data_.reference = new XDataReference(*(other.data_.reference));
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

XDataValue::XDataValue(const XDataValue &other) : type_(kInteger), array_type_(false) {
  operator =(other);
}

XDataValue::~XDataValue() {
  FreeData();
}

void XDataValue::FreeData() {
  if (array_type_) {
    switch (type_) {
      case kInteger:
      case kFloat:
        break;
      case kString:
        for (auto &i : *(data_.array_value)) {
          delete i.string_value;
        }
        break;
      case kNode:
        for (auto &i : *(data_.array_value)) {
          delete i.nested_value;
        }
        break;
      default:
        Assert(false);
        break;
    }
    delete data_.array_value;
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
    data_.array_value = new ArrayData();
    switch (type_) {
      case kInteger:
      case kFloat:
        data_.array_value->assign(other.data_.array_value->begin(),
                                  other.data_.array_value->end());
        break;
      case kString:
        data_.array_value->reserve(other.data_.array_value->size());
        for (auto &i : *(other.data_.array_value)) {
          Data curr;
          curr.string_value = new string(*(i.string_value));
          data_.array_value->push_back(move(curr));
        }
        break;
      case kNode:
        data_.array_value->reserve(other.data_.array_value->size());
        for (auto &i : *(other.data_.array_value)) {
          Data curr;
          curr.nested_value = new NestedData(*(i.nested_value));
          data_.array_value->push_back(move(curr));
        }
        break;
      default:
        Assert(false);
    }
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

bool ValidateTemplate(const XTemplate *nested, vector<shared_ptr<XDataValue>>::iterator &in,
                      vector<shared_ptr<XDataValue>>::iterator end, vector<shared_ptr<XDataValue>> &out);

bool ValidateData(const XTemplateMember *member, vector<shared_ptr<XDataValue>>::iterator &in,
                  vector<shared_ptr<XDataValue>>::iterator end, XDataValue::Data &out) {
  if (in == end) return false;
  switch (member->basic_type()) {
    case XTemplateMember::kInteger:
      if ((*in)->array_type()) return false;
      if ((*in)->type() == XDataValue::kInteger) out.int_value = (*in)->data().int_value;
      else if ((*in)->type() == XDataValue::kFloat) out.int_value = (int)(*in)->data().float_value;
      else return false;
      ++in;
      break;
    case XTemplateMember::kFloat:
      if ((*in)->array_type()) return false;
      if ((*in)->type() == XDataValue::kInteger) out.float_value = (*in)->data().int_value;
      else if ((*in)->type() == XDataValue::kFloat) out.float_value = (*in)->data().float_value;
      else return false;
      ++in;
      break;
    case XTemplateMember::kString:
      if ((*in)->array_type()) return false;
      if ((*in)->type() != XDataValue::kString) return false;
      out.string_value = new string(*((*in)->data().string_value));
      ++in;
      break;
    case XTemplateMember::kNode:
      out.nested_value = new XDataValue::NestedData();
      if (!ValidateTemplate(member->template_reference()->ptr, in, end, *(out.nested_value)))
        return false;
      break;
    default:
      Assert(false);
  }
  return true;
}

bool ValidateTemplate(const XTemplate *nested, vector<shared_ptr<XDataValue>>::iterator &in,
                      vector<shared_ptr<XDataValue>>::iterator end, vector<shared_ptr<XDataValue>> &out) {
  for (const auto &member : nested->members) {
    XDataValue::Type type = XDataValue::kInteger;
    switch (member->basic_type()) {
      case XTemplateMember::kInteger:
        type = XDataValue::kInteger;
        break;
      case XTemplateMember::kFloat:
        type = XDataValue::kFloat;
        break;
      case XTemplateMember::kString:
        type = XDataValue::kString;
        break;
      case XTemplateMember::kNode:
        type = XDataValue::kNode;
        break;
      default:
        Assert(false);
        break;
    }
    XDataValue *value = new XDataValue(type, member->member_type() == XTemplateMember::kArray);
    if (value->array_type()) {
      int size = 1;
      for (const auto &i : *(member->array_size())) {
        switch (i.type()) {
          case XTemplateMember::ArraySize::kStatic: {
            size *= i.data().size;
            break;
          } case XTemplateMember::ArraySize::kDynamic: {
            size_t pos = i.data().reference->pos;
            Assert(pos < out.size());
            size *= out[pos]->data().int_value;
            break;
          } default:
            Assert(false);
        }
      }
      for (int i = 0; i < size; ++i) {
        XDataValue::Data data;
        if (!ValidateData(member.get(), in, end, data)) return false;
        value->data().array_value->push_back(move(data));
      }
    } else {
      if (!ValidateData(member.get(), in, end, value->data())) return false;
    }

    out.push_back(shared_ptr<XDataValue>(value));
  }
  return true;
}

bool XData::Validate(XFile *file) {
  auto ti = file->templates().find(template_id);
  if (ti == file->templates().end())
    return false;
  template_ptr = ti->second.get();
  vector<shared_ptr<XDataValue>> new_data;
  auto di = data.begin();
  if (!ValidateTemplate(template_ptr, di, data.end(), new_data))
    return false;
  
  switch (template_ptr->restriction_type) {
    case XTemplate::kClosed:
      if (!nested_data.empty())
        return false;
      break;
    case XTemplate::kRestricted:
      for (auto &i : nested_data) {
        bool found = false;
        switch (i.type()) {
          case XNestedData::kNode:
            for (auto &j : *(template_ptr->restrictions)) {
              if (j.id == i.data().node->template_id) {
                found = true;
                break;
              }
            }
            break;
          case XNestedData::kNodeReference:
            // TODO: not supported
            return false;
          default:
            Assert(false);
        }
        if (!found)
          return false;
      }
    case XTemplate::kOpened:
      break;
    default:
      Assert(false);
      break;
  }

  data = new_data;
  return true;
}

}
