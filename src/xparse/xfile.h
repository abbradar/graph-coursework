#ifndef XPARSE_XFILE_H_
#define XPARSE_XFILE_H_

#include <map>
#include <vector>
#include <string>
#include <istream>
#include <memory>
#include "xtemplate.h"
#include "xdata.h"

namespace xparse {

struct XTemplate;
struct XData;

class XFile {
 public:
  typedef std::map<std::string, std::unique_ptr<XTemplate>> TemplateMap;
  typedef std::vector<std::unique_ptr<XData>> NodeVector;

  XFile();

  inline TemplateMap &templates() {
    return templates_;
  }

  inline NodeVector &data_nodes() {
    return data_nodes_;
  }

  bool Parse(std::istream &in);

 private:
  TemplateMap templates_;
  NodeVector data_nodes_;
};

}

#endif // XPARSE_XFILE_H_
