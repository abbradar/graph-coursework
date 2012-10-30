#ifndef XPARSE_XDATA_H_
#define XPARSE_XDATA_H_

#include <string>
#include <vector>
#include "guid.h"
#include "xfile.h"
#include "xtemplate.h"

namespace xparse {

class XFile;
class XTemplate;
struct XData;

struct XDataReference {
  std::string id;
  GUID guid;
  XData *ptr;

  bool Resolve(XFile *file);
};

class XNestedData {
 public:
  enum Type {
    kNode, kNodeReference
  };

  union Data {
    XData *node;
    XDataReference *reference;
  };

  XNestedData(Type type);
  XNestedData(const XNestedData &other);
  ~XNestedData();

  XNestedData &operator =(const XNestedData &other);

  inline Type type() {
    return type_;
  }

  inline Data &data() {
    return data_;
  }

 private:
  void FreeData();

  Type type_;
  Data data_;
};

class XDataValue {
 public:
  typedef std::vector<XDataValue> NestedData;

  enum Type {
    kInteger, kFloat, kString, kNode, kNested
  };

  union Data {
    int int_value;
    float float_value;
    std::string *string_value;
    std::vector<Data> *array_value;
    NestedData *nested_value;
  };

  typedef std::vector<Data> ArrayData;

  XDataValue(Type type, bool array_type = false);
  XDataValue(const XDataValue &other);
  ~XDataValue();

  XDataValue &operator =(const XDataValue &other);

  inline Type type() {
    return type_;
  }

  inline bool array_type() {
    return array_type_;
  }

  inline Data &data() {
    return data_;
  }

 private:
  void FreeData();

  Type type_;
  bool array_type_;
  Data data_;
};

struct XData {
  std::string template_id;
  XTemplate *template_ptr;
  std::string id;
  GUID guid;
  std::vector<XDataValue> data;
  std::vector<XNestedData> nested_data;

  bool Validate(XFile *file);
};

}

#endif // XPARSE_XDATA_H_
