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

  inline Type type() const {
    return type_;
  }

  inline const Data &data() const {
    return data_;
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
  union Data;

  typedef std::vector<std::shared_ptr<XDataValue>> NodeData;
  typedef std::vector<Data> ArrayData;

  enum Type {
    kInteger, kFloat, kString, kNode
  };

  union Data {
    int int_value;
    float float_value;
    std::string *string_value;
    NodeData *node_value;
    ArrayData *array_value;
  };

  XDataValue(Type type, bool array_type = false);
  XDataValue(const XDataValue &other);
  ~XDataValue();

  XDataValue &operator =(const XDataValue &other);

  inline Type type() const {
    return type_;
  }

  inline bool array_type() const {
    return array_type_;
  }

  inline const Data &data() const {
    return data_;
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
  std::vector<std::shared_ptr<XDataValue>> data;
  std::vector<XNestedData> nested_data;

  bool Validate(XFile *file);
};

}

#endif // XPARSE_XDATA_H_
