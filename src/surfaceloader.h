#ifndef GRAPH_SURFACELOADER_H_
#define GRAPH_SURFACELOADER_H_

#include <string>
#include "sdlobj/surface.h"

class SurfaceLoader {
 public:
  SurfaceLoader();
  virtual ~SurfaceLoader();

  virtual bool Ready() = 0;
  virtual sdlobj::Surface LoadSurface() = 0;
};

class FileSurfaceLoader : public SurfaceLoader {
 public:
  FileSurfaceLoader();
  ~FileSurfaceLoader();

  bool Ready();
  sdlobj::Surface LoadSurface();

  inline const std::string &path() const {
    return path_;
  }

  void set_path(const std::string &path);

 private:
  std::string path_;
};

#endif // GRAPH_SURFACELOADER_H_
