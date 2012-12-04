#ifndef GRAPH_CAMERA_H_
#define GRAPH_CAMERA_H_

#include "position.h"

class Camera : public Position {
 public:
  Camera();

  inline myfloat width() const {
    return width_;
  }

  inline myfloat height() const {
    return height_;
  }

  void set_size(const myfloat width, const myfloat height);

  inline myfloat viewer_distance() const {
    return viewer_distance_;
  }

  void set_viewer_distance(const myfloat viewer_distance);
  
  inline myfloat scale() const {
    return scale_;
  }

  void set_scale(const myfloat scale);

  inline myfloat view_limit() const {
    return view_limit_;
  }

  void set_view_limit(const myfloat view_limit);

  void PerspectiveTransform(Point3D &point) const;
  void ReversePerspectiveTransform(Point3D &point) const;

 private:
  myfloat viewer_distance_;
  myfloat view_limit_;
  myfloat scale_;
  myfloat width_;
  myfloat height_;
  myfloat k_;
};

#endif // GRAPH_CAMERA_H_
