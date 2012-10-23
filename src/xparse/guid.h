#ifndef XPARSE_GUID_H_
#define XPARSE_GUID_H_

#include <string>

namespace xparse {

class GUID {
 public:
  GUID();
  GUID(const char *guid);
  GUID(const GUID &other);

  static constexpr int kGUIDSize = 16;

  //std::string ToString() const;
  
  inline char *guid() {
    return guid_;
  }

 private:
  char guid_[kGUIDSize];
}

}

#endif
