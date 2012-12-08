#ifndef GRAPH_MULTILINELABEL_H_
#define GRAPH_MULTILINELABEL_H_

#include <vector>
#include "textcontrol.h"
#include "cacheddrawer.h"

class MultiLineLabel : public TextControl, public CachedDrawer {
 public:
  MultiLineLabel();
  ~MultiLineLabel();

  inline const std::vector<std::string> &items() {
    return items_;
  }

  void set_items(const std::vector<std::string> &items);

  unsigned preferred_height();
  unsigned preferred_width();

 protected:
  void Repaint(sdlobj::Surface &surface);

 private:
  std::vector<std::string> items_;
};

#endif // GRAPH_MULTILINELABEL_H_
