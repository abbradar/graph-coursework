#ifndef GRAPH_MATH_H_
#define GRAPH_MATH_H_

const float kPi = 3.14159265f;

template <typename T> int sign(T val) {
    return (T(0) < val) - (val < T(0));
}

#endif // GRAPH_MATH_H_
