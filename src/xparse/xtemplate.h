#ifndef XPARSE_XTEMPLATE_H_
#define XPARSE_XTEMPLATE_H_

#include <cstring>
#include <map>
#include <string>
#include <vector>
#include <memory>
#include "guid.h"
#include "xfile.h"

namespace xparse {

class XFile;
struct XTemplate;

struct XTemplateReference {
  std::string id;
  GUID guid;
  XTemplate *ptr;

  bool Resolve(XFile *file);
};

class XTemplateMember {
 public:
  enum MemberType {
    kBasic, kArray
  };
  enum BasicType {
    kInteger, kFloat, kString, kNode
  };

  XTemplateMember(MemberType member_type, BasicType basic_type);
  XTemplateMember(const XTemplateMember &other);
  ~XTemplateMember();

  XTemplateMember &operator =(const XTemplateMember &other);

  inline std::string &id() {
    return id_;
  }

  inline const std::string &id() const {
    return id_;
  }

  inline MemberType member_type() const {
    return member_type_;
  }

  inline BasicType basic_type() const {
    return basic_type_;
  }

  inline XTemplateReference *template_reference() {
    return template_reference_.get();
  }

  inline const XTemplateReference *template_reference() const {
    return template_reference_.get();
  }

  inline std::vector<size_t> *array_size() {
    return array_size_.get();
  }

  inline const std::vector<size_t> *array_size() const {
    return array_size_.get();
  }

 private:
  std::string id_;
  MemberType member_type_;
  BasicType basic_type_;
  std::unique_ptr<XTemplateReference> template_reference_;
  std::unique_ptr<std::vector<size_t>> array_size_;
};

struct XTemplate {
  enum RestrictionType {
    kOpened, kClosed, kRestricted
  };
  
  std::string id;
  GUID guid;
  std::vector<XTemplateMember> members;
  RestrictionType restriction_type;
  std::unique_ptr<std::vector<XTemplateReference>> restrictions;
};

}

#endif // XPARSE_XTEMPLATE_H_
