#ifndef XPARSE_XFILE_H_
#define XPARSE_XFILE_H_

#include <map>
#include <vector>
#include <string>
#include <istream>
#include "xtemplate.h"
#include "xnode.h"

namespace xparse {

class XFile {
 public:
  typedef std::map<std::string, XTemplate> TemplateMap;
  typedef std::vector<XNode> NodeVector;

  XFile();

  inline TemplateMap &templates() {
    return templates_;
  }

  inline NodeVector data_nodes() {
    return data_nodes_;
  }

  bool Parse(std::istream &in);

 private:
  TemplateMap templates_;
  NodeVector data_nodes_;
};

}

#endif // XPARSE_XFILE_H_
