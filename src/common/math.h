#ifndef COMMON_MATH_H_
#define COMMON_MATH_H_

extern const float kPi;

template <class T> char sign(const T val) {
    return (char)((T(0) < val) - (val < T(0)));
}

template <class T> T trim(const T val, const T min, const T max) {
  if (val < min) return min;
  else if (val > max) return max;
  else return val;
}

template <class T> T circle(T val, const T min, const T max) {
  while (val > max) val -= (max - min);
  while (val < min) val += (max - min);
  return val;
}

#endif // COMMON_MATH_H_
