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
struct XTemplateMember;

// TODO: make them classes with necessary protection and stuff
struct XTemplateReference {
  std::string id;
  GUID guid;
  XTemplate *ptr;

  bool Resolve(XFile *file);
};

struct XTemplateMemberReference {
  std::string id;
  XTemplateMember *ptr;
  size_t pos;

  bool Resolve(XTemplate *x_template);
};

class XTemplateMember {
 public:
  class ArraySize {
   public:
    enum Type {
      kStatic, kDynamic
    };

    union Data {
      size_t size;
      XTemplateMemberReference *reference;
    };

    ArraySize(Type type);
    ArraySize(const ArraySize &other);
    ~ArraySize();

    ArraySize &operator =(const ArraySize &other);

    inline Type type() const {
      return type_;
    }

    inline Data &data() {
      return data_;
    }

    inline const Data &data() const {
      return data_;
    }

   private:
    void FreeData();

    Type type_;
    Data data_;
  };

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

  inline std::vector<ArraySize> *array_size() {
    return array_size_.get();
  }

  inline const std::vector<ArraySize> *array_size() const {
    return array_size_.get();
  }

 private:
  std::string id_;
  MemberType member_type_;
  BasicType basic_type_;
  std::unique_ptr<XTemplateReference> template_reference_;
  std::unique_ptr<std::vector<ArraySize>> array_size_;
};

struct XTemplate {
  enum RestrictionType {
    kOpened, kClosed, kRestricted
  };
  
  std::string id;
  GUID guid;
  std::vector<std::shared_ptr<XTemplateMember>> members;
  RestrictionType restriction_type;
  std::unique_ptr<std::vector<XTemplateReference>> restrictions;
};

}

#endif // XPARSE_XTEMPLATE_H_
