#ifndef COMMON_MATH_H_
#define COMMON_MATH_H_

#include <cmath>

// begin paste from GNU libc math.h
# define M_E		2.7182818284590452354	/* e */
# define M_LOG2E	1.4426950408889634074	/* log_2 e */
# define M_LOG10E	0.43429448190325182765	/* log_10 e */
# define M_LN2		0.69314718055994530942	/* log_e 2 */
# define M_LN10		2.30258509299404568402	/* log_e 10 */
# define M_PI		3.14159265358979323846	/* pi */
# define M_PI_2		1.57079632679489661923	/* pi/2 */
# define M_PI_4		0.78539816339744830962	/* pi/4 */
# define M_1_PI		0.31830988618379067154	/* 1/pi */
# define M_2_PI		0.63661977236758134308	/* 2/pi */
# define M_2_SQRTPI	1.12837916709551257390	/* 2/sqrt(pi) */
# define M_SQRT2	1.41421356237309504880	/* sqrt(2) */
# define M_SQRT1_2	0.70710678118654752440	/* 1/sqrt(2) */
// end paste

template <class T> inline char Sign(const T val) {
    return (char)((T(0) < val) - (val < T(0)));
}

template <class T> T Trim(const T val, const T min, const T max) {
  if (val < min) return min;
  else if (val > max) return max;
  else return val;
}

template <class T> T Circle(T val, const T min, const T max) {
  while (val > max) val -= (max - min);
  while (val < min) val += (max - min);
  return val;
}

template <class T> inline T PositiveRound(const T val) {
  return floor(val + T(0.5));
}

template <class T> inline T Round(const T val) {
  return (val > T(0.0)) ? floor(val + T(0.5)) : ceil(val - T(0.5));
}

template <class T> inline T Sqr(const T val) {
  return val * val;
}

template <class T> void LinearInterpolation(const T x1, const T y1, const T x2, const T y2,
                                            T &a, T &b) {
  if (x1 == x2) {
    a = 0;
    b = (y1 + y2) / 2;
    return;
  }
  a = (y1 - y2) / (x1 - x2);
  b = y1 - a * x1;
}

template <class T> void QuadraticInterpolation(const T x1, const T y1, const T x2, const T y2,
                                               const T x3, const T y3, T &a, T &b, T &c) {
  if (x1 == x2) {
    a = 0;
    LinearInterpolation(x1, y1, x3, y3, b, c);
    return;
  }
  if (x2 == x3) {
    a = 0;
    LinearInterpolation(x1, y1, x2, y2, b, c);
    return;
  }
  if (x1 == x3) {
    a = 0;
    LinearInterpolation(x2, y2, x3, y3, b, c);
    return;
  }
  a = ((y3 - y1) * (x2 - x1) - (y2 - y1) * (x3 - x1)) /
      ((Sqr(x3) - Sqr(x1)) * (x2 - x1) - (Sqr(x2) - Sqr(x1)) * (x3 - x1));
  b = (y2 - y1 - a * (Sqr(x2) - Sqr(x1))) / (x2 - x1);
  c = y1 - (a * Sqr(x1) + b * x1);
}

template <class T> inline T QuadraticFunc(const T a, const T b, const T c, const T x) {
  return a * Sqr(x) + b * x + c;
}

template <class T> inline void WideBounds(const T value, T &min, T &max) {
  if (min > value) min = value;
  else if (max < value) max = value;
}

#endif // COMMON_MATH_H_
