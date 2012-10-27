#ifndef XPARSE_XTEMPLATE_H_
#define XPARSE_XTEMPLATE_H_

#include <cstring>
#include <map>
#include <string>
#include <vector>
#include "guid.h"

namespace xparse {

class XTemplateMember {
 public:
  enum MemberType {
    kBasic, kArray, kTemplate
  };
  enum BasicType {
    kInteger, kFloat, kString
  };
  union TypeData {
    BasicType basic_type;
    struct {
      BasicType basic_type;
      std::vector<size_t> *size;
    } array_type;
    std::string *template_type;
  };

  XTemplateMember(MemberType type);
  XTemplateMember(const XTemplateMember &other);
  ~XTemplateMember();

  XTemplateMember &operator =(const XTemplateMember &other);

  inline std::string &id() {
    return id_;
  }

  inline MemberType type() {
    return type_;
  }

  inline TypeData &data() {
    return data_;
  }

 private:
  void FreeData();

  std::string id_;
  MemberType type_;
  TypeData data_;
};

struct XTemplate {
  std::string id;
  GUID guid;
  std::map<std::string, XTemplateMember> members;
};

}

#endif // XPARSE_XTEMPLATE_H_
