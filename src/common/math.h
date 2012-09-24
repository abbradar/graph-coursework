#ifndef COMMON_MATH_H_
#define COMMON_MATH_H_

const float kPi = 3.14159265f;

template <typename T> int sign(T val) {
    return (T(0) < val) - (val < T(0));
}

#endif // COMMON_MATH_H_
