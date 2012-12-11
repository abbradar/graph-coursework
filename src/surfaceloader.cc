#include <fstream>
#include "sdlobj/sdlimage.h"
#include "common/exception.h"
#include "surfaceloader.h"

using namespace std;

SurfaceLoader::SurfaceLoader() = default;
SurfaceLoader::~SurfaceLoader() = default;

FileSurfaceLoader::FileSurfaceLoader() = default;
FileSurfaceLoader::~FileSurfaceLoader() = default;

bool FileSurfaceLoader::Ready() {
  ifstream test(path_);
  return test.good();
}

sdlobj::Surface FileSurfaceLoader::LoadSurface() {
  unique_ptr<ifstream> in(new ifstream(path_));
  if (!in->good()) throw Exception("Cannot open surface file: " + path_);
  return sdlobj::SDLImage::Load(move(in));
}

void FileSurfaceLoader::set_path(const std::string &path) {
  path_ = path;
}
